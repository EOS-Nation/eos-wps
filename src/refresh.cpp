[[eosio::action]]
void wps::refresh( const name voter )
{
    check( !is_voter_eligible( voter, get_eligible_producers() ), "voter is eligible, no change");

    check( remove_voter( voter ), "voter has no votes to remove");

    // update `proposals::eligible` field for all active proposals
    update_eligible_proposals();
}

bool wps::refresh_proposal( const name proposal_name, const set<name> eligible_producers )
{
    auto votes_itr = _votes.find( proposal_name.value );
    bool removed = false;

    _votes.modify( votes_itr, same_payer, [&]( auto& row ) {
        // iterate over each vote
        for (std::pair<eosio::name, eosio::name> item : votes_itr->votes) {
            // remove votes from voters not eligible
            if ( !is_voter_eligible( item.first, eligible_producers ) ) {
                row.votes.erase( voter );
                removed = true;
            }
        }
        if ( removed ) update_total_net_votes( proposal_name, row.votes );
    });
    return removed;
}

bool wps::remove_voter( const name voter )
{
    // settings
    auto settings = _settings.get();
    auto state = _state.get();
    bool removed = false;

    // iterate proposals by active status
    for ( auto proposal_name : group_proposals( "active"_n ) ) {
        auto votes_itr = _votes.find( proposal_name.value );

        // iterate over each vote
        for (std::pair<eosio::name, eosio::name> item : votes_itr->votes) {
            // voter is voting for proposal
            if ( item.first == voter ) {
                _votes.modify( votes_itr, same_payer, [&]( auto& row ) {
                    row.votes.erase( voter );
                    update_total_net_votes( proposal_name, row.votes );
                });
                removed = true;
            }
        }
    }
    return removed;
}

bool wps::is_voter_eligible( const name voter, const set<name> eligible_producers )
{
    return eligible_producers.find( voter ) != eligible_producers.end();
}

set<name> wps::get_eligible_producers()
{
    eosiosystem::producers_table _producers( "eosio"_n, "eosio"_n.value );
    eosiosystem::global_state_singleton _gstate( "eosio"_n, "eosio"_n.value );

    auto producer_by_votes = _producers.get_index<"prototalvote"_n>();
    auto gstate = _gstate.get();

    set<name> eligible_producers;

    for ( auto producer : producer_by_votes ) {
        const int64_t producer_per_vote_pay = int64_t((gstate.pervote_bucket * producer.total_votes) / gstate.total_producer_vote_weight);
        if ( producer_per_vote_pay < 1000000 ) break;
        eligible_producers.insert( producer.owner );
    }
    return eligible_producers;
}