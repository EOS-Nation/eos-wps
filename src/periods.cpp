void wps::proposal_to_periods( const eosio::name proposal_name, const eosio::time_point_sec start_voting_period, const uint8_t duration, const eosio::name ram_payer )
{
    // settings
    auto state = _state.get();
    auto settings = _settings.get();

    // insert proposal name into multiple periods
    for (int i = 0; i < duration; i++) {
        const eosio::time_point_sec voting_period = time_point(start_voting_period) + time_point_sec(settings.voting_interval * i);
        auto periods_itr = _periods.find( voting_period.sec_since_epoch() );

        // create new set of proposals
        if ( periods_itr == _periods.end() ) {
            _periods.emplace( ram_payer, [&]( auto& row ) {
                row.voting_period   = voting_period;
                row.proposals       = std::set<eosio::name> { proposal_name };
            });
        // insert proposal to old ones
        } else {
            _periods.modify( periods_itr, ram_payer, [&]( auto& row ) {
                row.proposals.insert( proposal_name );
                check(row.proposals.size() <= 100, "cannot exceed 100 proposals per single voting period");
            });
        }
    }
}
