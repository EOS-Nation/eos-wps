// @action
void wps::vote( const eosio::name voter, const eosio::name proposal_name, const eosio::name vote )
{
    require_auth( voter );
    const eosio::name ram_payer = get_self();

    // validation
    auto proposals_itr = _proposals.find( proposal_name.value );
    auto votes_itr = _votes.find( proposal_name.value );

    check( proposals_itr != _proposals.end(), "[proposal_name] does not exists");
    check( proposals_itr->start < current_time_point(), "[proposal_name] has not yet started");
    check( proposals_itr->end > current_time_point(), "[proposal_name] has ended");
    check( votes_itr != _votes.end(), "[proposal_name] votes does not exist");
    check( vote == "yes"_n || vote == "no"_n || vote == "abstain"_n, "[vote] invalid (ex: yes/no/abstain)");

    // update vote
    _votes.modify( votes_itr, ram_payer, [&]( auto& vote_row ) {
        check(vote_row.votes[voter] != vote, "[vote] has not been modified");
        vote_row.votes[voter] = vote;

        // re-caculate votes
        _proposals.modify( proposals_itr, ram_payer, [&]( auto& proposals_row ) {
            proposals_row.total_net_votes = calculate_total_net_votes( vote_row.votes );
        });
    });
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
