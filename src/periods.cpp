void wps::check_max_number_proposals()
{
    auto periods_itr = _periods.find( _state.get().current_voting_period.sec_since_epoch() );
    if ( periods_itr == _periods.end() ) return; // skip if no proposals in current period
    check(periods_itr->proposals.size() <= 100, "cannot exceed 100 proposals per single voting period");
}

void wps::add_proposal_to_periods( const name proposal_name )
{
    const name ram_payer = get_self();
    auto state = _state.get();
    auto periods_itr = _periods.find( state.current_voting_period.sec_since_epoch() );

    // create new set of proposals
    if ( periods_itr == _periods.end() ) {
        _periods.emplace( ram_payer, [&]( auto& row ) {
            row.voting_period   = state.current_voting_period;
            row.proposals       = set<name> { proposal_name };
        });
    // insert proposal to old ones
    } else {
        _periods.modify( periods_itr, ram_payer, [&]( auto& row ) {
            row.proposals.insert( proposal_name );
        });
    }
}

void wps::copy_current_to_next_periods()
{
    const name ram_payer = get_self();
    auto state = _state.get();
    auto settings = _settings.get();

    // must calculate next_voting_period in case the next period has been changed caused by delayed complete action
    // cannot use `state.next_voting_period`
    const time_point_sec next_voting_period = current_time_point() + time_point_sec( settings.voting_interval );

    // lookup iterators
    auto current_periods_itr = _periods.find( state.current_voting_period.sec_since_epoch() );
    auto next_periods_itr = _periods.find( next_voting_period.sec_since_epoch() );

    // create new set of proposals
    if ( next_periods_itr == _periods.end() ) {
        _periods.emplace( ram_payer, [&]( auto& row ) {
            row.voting_period   = next_voting_period;
            row.proposals       = current_periods_itr->proposals;
        });
    // insert proposal to old ones
    } else {
        _periods.modify( next_periods_itr, ram_payer, [&]( auto& row ) {
            row.proposals = current_periods_itr->proposals;
        });
    }
}