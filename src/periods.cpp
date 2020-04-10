void wps::check_max_number_proposals()
{
    auto periods_itr = _periods.find( _state.get().current_voting_period.sec_since_epoch() );
    if ( periods_itr == _periods.end() ) return; // skip if no proposals in current period
    check(periods_itr->proposals.size() <= 100, "cannot exceed 100 proposals per single voting period");
}

void wps::add_proposal_to_periods( const name proposal_name, const time_point_sec voting_period )
{
    const name ram_payer = get_self();
    auto periods_itr = _periods.find( voting_period.sec_since_epoch() );

    // create new set of proposals
    if ( periods_itr == _periods.end() ) {
        _periods.emplace( ram_payer, [&]( auto& row ) {
            row.voting_period   = voting_period;
            row.proposals       = set<name> { proposal_name };
        });
    // insert proposal to old ones
    } else {
        _periods.modify( periods_itr, ram_payer, [&]( auto& row ) {
            row.proposals.insert( proposal_name );
        });
    }
}

void wps::copy_active_voting_periods( const time_point_sec current_voting_period, const time_point_sec next_voting_period )
{
    const name ram_payer = get_self();

    // iterators
    auto current_periods_itr = _periods.find( current_voting_period.sec_since_epoch() );
    auto next_periods_itr = _periods.find( next_voting_period.sec_since_epoch() );

    // filter only active proposals from current preiod
    set<name> active_proposals;
    for ( const name proposal_name : current_periods_itr->proposals ) {
        if ( _proposals.get( proposal_name.value ).status == "active"_n) {
            active_proposals.insert( proposal_name );
        }
    }

    // create new set of proposals
    if ( next_periods_itr == _periods.end() ) {
        _periods.emplace( ram_payer, [&]( auto& row ) {
            row.voting_period   = next_voting_period;
            row.proposals       = active_proposals;
        });
    // insert proposal to old ones
    } else {
        _periods.modify( next_periods_itr, ram_payer, [&]( auto& row ) {
            row.proposals = active_proposals;
        });
    }
}