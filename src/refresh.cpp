[[eosio::action]]
void wps::refresh( const name voter )
{
    check( !is_voter_eligible( voter ), "voter is eligible, no change");

    check( remove_voter( voter ), "voter has no votes to remove");

    // update `proposals::eligible` field for all active proposals
    update_eligible_proposals();
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
                    delete row.votes[ voter ];
                    update_total_net_votes( proposal_name, row.votes );
                });
                removed = true;
            }
        }
    }
    return removed;
}

bool wps::is_voter_eligible( const name voter )
{
    eosiosystem::producers_table _producers( "eosio"_n, "eosio"_n.value );
    eosiosystem::global_state_singleton _gstate( "eosio"_n, "eosio"_n.value );

    auto prod_itr = _producers.find( voter.value );

    // "[voter] must be registered as a producer"
    if ( prod_itr == _producers.end() ) return false;

    auto gstate = _gstate.get();
    const int64_t producer_per_vote_pay = int64_t((gstate.pervote_bucket * prod_itr->total_votes) / gstate.total_producer_vote_weight);

    // "[voter] must be have a vpay of 100 EOS or above"
    if ( producer_per_vote_pay < 1000000 ) return false;

    return true;
}