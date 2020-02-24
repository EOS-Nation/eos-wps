[[eosio::action]]
void wps::complete( )
{
    // no authorization required (can be executed by any account)

    // check if current voting period is completed
    check( is_voting_period_complete(), "[current_voting_period] is not completed");

    // update `proposals::eligible` field for all active proposals
    update_eligible_proposals();

    // payouts of active proposals
    handle_payouts();

    // set pending proposals to active status
    set_pending_to_active();

    // update current & next voting period
    update_to_next_voting_period();

    // re-update `proposals::eligible`
    update_eligible_proposals();

    // `complete` action must be manually executed for the next `next_voting_period`
}

bool wps::is_voting_period_complete()
{
    return current_time_point() >= time_point( _state.get().next_voting_period );
}

void wps::update_to_next_voting_period()
{
    auto state = _state.get();
    auto settings = _settings.get();

    state.current_voting_period = state.next_voting_period;
    state.next_voting_period = state.next_voting_period + settings.voting_interval;
    _state.set( state, same_payer );
}

void wps::handle_payouts()
{
    // settings
    auto settings = _settings.get();
    auto state = _state.get();

    // static actions
    wps::claim_action claim( get_self(), { get_self(), "active"_n });

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

            // push 0 second deferred transaction to auto-execute claim
            const uint64_t key = proposal_name.value + "claim"_n.value;
            send_deferred( claim.to_action( proposal_name ), key, 0 );
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
        // proposal which are no longer active during the next voting period
        if ( !proposal_exists_per_voting_period( proposal_name, state.next_voting_period )) {
            if ( row.payouts == row.total_budget ) row.status = "completed"_n;
            else if ( row.payouts.amount > 0 ) row.status = "partial"_n;
            else row.status = "expired"_n;
            row.eligible = false;
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
    auto index = _proposals.get_index<"bystatus"_n>();
    auto itr = index.find("pending"_n.value);

    if (itr == index.end()) return;

    index.modify( itr, same_payer, [&]( auto& row ) {
        row.status = "active"_n;
    });
}

void wps::check_voting_period_completed()
{
    auto state = _state.get();
    check( current_time_point() < time_point( state.next_voting_period ), "[current_voting_period] is completed, any account must execute [complete] ACTION to continue");
}