[[eosio::action]]
void wps::vote( const eosio::name voter, const eosio::name proposal_name, const eosio::name vote )
{
    require_auth( voter );

    // validate proposal if eligible to vote on
    check_proposal_can_vote( proposal_name );

    // cannot vote during completed voting period phase
    check_voting_period_completed();

    // get BP stats
    update_producer( voter );

    // update `votes` table
    update_vote( voter, proposal_name, vote );

    // update `proposals::eligible` field for all active proposals
    update_eligible_proposals();
}

// TESTING PURPOSES: to test voting as producer
[[eosio::action]]
void wps::voteproducer( const eosio::name voter, const eosio::name proposal_name, const eosio::name vote )
{
    require_auth( voter );

    // get BP stats
    update_producer( voter );

    // must be producer & claimed within last 24 hours
    check_voter_eligible( voter );

    wps::vote( voter, proposal_name, vote );
}

void wps::check_voter_eligible( const eosio::name voter )
{
    producers_table _producers( "eosio"_n, "eosio"_n.value );
    auto producers_itr = _producers.find( voter.value );

    const eosio::time_point last_24h = time_point(current_time_point() - time_point_sec(DAY));
    check( producers_itr != _producers.end(), "[voter] must be registered as a producer");
    check( producers_itr->last_claim_time >= last_24h, "[voter] must have claimed producer rewards within the last 24 hours");
}

void wps::check_proposal_can_vote( const eosio::name proposal_name )
{
    auto proposals_itr = _proposals.find( proposal_name.value );
    check( proposals_itr != _proposals.end(), "[proposal_name] does not exists");
    check( proposals_itr->start_voting_period <= current_time_point(), "[proposal_name] has not yet started");
    check( proposals_itr->end > current_time_point(), "[proposal_name] has ended");
}

int16_t wps::calculate_total_net_votes( const std::map<eosio::name, eosio::name> votes )
{
    int16_t total_net_votes = 0;
    for (std::pair<eosio::name, eosio::name> item : votes) {
        const eosio::name vote = item.second;
        if (vote == "yes"_n) total_net_votes += 1;
        else if (vote == "no"_n) total_net_votes -= 1;
    }
    return total_net_votes;
}

void wps::update_vote( const eosio::name voter, const eosio::name proposal_name, const eosio::name vote )
{
    // validate vote
    auto votes_itr = _votes.find( proposal_name.value );
    auto proposals_itr = _proposals.find( proposal_name.value );

    check( votes_itr != _votes.end(), "[proposal_name] votes does not exist");
    check( vote == "yes"_n || vote == "no"_n || vote == "abstain"_n, "[vote] invalid (ex: yes/no/abstain)");

    // update vote
    _votes.modify( votes_itr, same_payer, [&]( auto& vote_row ) {
        check(vote_row.votes[voter] != vote, "[vote] has not been modified");
        vote_row.votes[voter] = vote;

        // re-caculate votes
        _proposals.modify( proposals_itr, same_payer, [&]( auto& proposals_row ) {
            proposals_row.total_net_votes = calculate_total_net_votes( vote_row.votes );
        });
    });
}

void wps::update_eligible_proposals()
{
    // settings
    auto settings = _settings.get();
    auto state = _state.get();

    // containers
    eosio::asset total_payout = asset{ 0, symbol{ "EOS", 4 }};

    // filter out min voting threshold proposals
    std::map<int16_t, std::set<eosio::name>> proposals = sort_proposals_by_net_votes( "active"_n );

    // iterate proposals from highest to lowest net votes
    for ( auto itr = proposals.rbegin(); itr != proposals.rend(); ++itr ) {

        // iterate over proposals
        for ( auto proposal_name : itr->second ) {
            // proposal variables
            auto proposal_itr = _proposals.find( proposal_name.value );
            const int16_t total_net_votes = itr->first;
            const eosio::name proposer = proposal_itr->proposer;
            const eosio::asset monthly_budget = proposal_itr->monthly_budget;

            // min requirements for payouts
            const bool is_min_vote_margin = total_net_votes >= settings.vote_margin;
            const bool is_enough_budget = (total_payout + monthly_budget) <= settings.max_monthly_budget;

            // set eligible of proposal (true/false)
            _proposals.modify( proposal_itr, same_payer, [&]( auto& row ) {
                if ( is_enough_budget && is_min_vote_margin ) {
                    total_payout += monthly_budget;
                    row.eligible = true;
                } else {
                    row.eligible = false;
                }
            });
        }
    }
}

std::map<int16_t, std::set<eosio::name>> wps::sort_proposals_by_net_votes( const eosio::name status )
{
    auto index = _proposals.get_index<"bystatus"_n>();

    // add proposals to ordered map
    std::map<int16_t, std::set<eosio::name>> proposals;

    for ( auto itr = index.lower_bound(status.value); itr != index.upper_bound(status.value); ++itr ) {
        proposals[itr->total_net_votes].insert(itr->proposal_name);
    }
    return proposals;
}