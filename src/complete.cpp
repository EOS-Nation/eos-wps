[[eosio::action]]
void wps::complete( )
{
    // no authorization required (can be executed by any account)

    // is contract paused or not
    check_contract_active();

    // check if current voting period is completed
    check( is_voting_period_complete(), "[current_voting_period] is not completed");

    // check if account has enough funding
    check_available_funding();

    // update `votes` from eligible voters
    // any existing votes with voters with less than 100 EOS vpay will be removed
    refresh_proposals();

    // update `proposals::eligible` field for all active proposals
    update_eligible_proposals();

    // payouts of active proposals
    handle_payouts();

    // update current & next voting period
    update_to_next_voting_period();

    // set pending proposals to active status
    set_pending_to_active();

}

bool wps::is_voting_period_complete()
{
    return current_time_point() >= time_point( _state.get().next_voting_period );
}

void wps::update_to_next_voting_period()
{
    auto state = _state.get();
    auto settings = _settings.get();

    // // UNCOMMENT FOR PRODUCTION
    // // start of voting period will start at the nearest 00:00UTC
    // const uint64_t now = current_time_point().sec_since_epoch();
    // const time_point_sec current_voting_period = time_point_sec(now - now % DAY);

    if ( TESTING ) print("set `current_voting_period` to now");
    const time_point_sec current_voting_period = current_time_point();

    state.current_voting_period = current_voting_period;
    state.next_voting_period = state.current_voting_period + settings.voting_interval;
    _state.set( state, same_payer );
}

void wps::handle_payouts()
{
    // settings
    auto settings = _settings.get();
    auto state = _state.get();

    // iterate proposals by active status
    for ( auto proposal_name : group_proposals( "active"_n ) ) {
        auto proposals_itr = _proposals.find( proposal_name.value );
        const eosio::asset monthly_budget = proposals_itr->monthly_budget;

        // only payout eligible (true) proposals
        if ( proposals_itr->eligible ) {

            // check internal available funding
            check( state.available_funding >= monthly_budget, "insufficient `available_funding`");
            sub_funding( monthly_budget );

            // update proposal payouts
            _proposals.modify( proposals_itr, same_payer, [&]( auto& row ) {
                row.payouts += monthly_budget;
            });
        } else {
            // remove proposal that no longer met threshold, cancel all subsequent proposals
            _proposals.modify( proposals_itr, same_payer, [&]( auto& row ) {
                row.status = "expired"_n;
            });
        }

        // set proposals to `completed/partial/expired/active`
        update_proposal_status( proposal_name );
    }
}

std::set<eosio::name> wps::group_proposals( const eosio::name status )
{
    auto index = _proposals.get_index<"bystatus"_n>();

    // add proposals to ordered map
    std::set<eosio::name> proposals;

    for ( auto itr = index.lower_bound(status.value); itr != index.upper_bound(status.value); ++itr ) {
        proposals.insert(itr->proposal_name);
    }
    return proposals;
}

/**
 * status
 *
 * `active` => proposal still available for next voting period
 * `completed` => proposal payout in full
 * `partial` => proposal payout partially
 * `expired` => proposal did not receive any payout
 */
void wps::update_proposal_status( const eosio::name proposal_name )
{
    auto state = _state.get();
    auto proposal_itr = _proposals.find( proposal_name.value );

    _proposals.modify( proposal_itr, same_payer, [&]( auto& row ) {
        // reduce active voting period by 1
        if ( row.status == "active"_n && row.remaining_voting_periods > 0 ) row.remaining_voting_periods -= 1;

        // no more remaining voting periods
        if ( row.remaining_voting_periods <= 0 ) {
            if ( row.payouts == row.total_budget ) row.status = "completed"_n;
            else if ( row.payouts.amount > 0 ) row.status = "partial"_n;
            else row.status = "expired"_n;
            row.eligible = false;
        }

        // set finished proposals to 0 remaining voting periods
        if ( row.status == "completed"_n || row.status == "partial"_n || row.status == "expired"_n ) {
            row.remaining_voting_periods = 0;
        }
    });
}

bool wps::proposal_exists_per_voting_period( const eosio::name proposal_name, const eosio::time_point_sec voting_period )
{
    auto itr = _periods.find( voting_period.sec_since_epoch() );
    if ( itr == _periods.end()) return false;
    return itr->proposals.find( proposal_name ) != itr->proposals.end();
}

void wps::set_pending_to_active()
{
    for ( auto proposal_name : group_proposals( "pending"_n ) ) {
        auto proposals_itr = _proposals.find( proposal_name.value );

        _proposals.modify( proposals_itr, same_payer, [&]( auto& row ) {
            row.status = "active"_n;
            row.start_voting_period = _state.get().current_voting_period;
        });
    }
}

void wps::check_voting_period_completed()
{
    auto state = _state.get();
    check( current_time_point() < time_point( state.next_voting_period ), "[current_voting_period] is completed, any account must execute [complete] ACTION to continue");
}
