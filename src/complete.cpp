// @action
void wps::complete( const eosio::time_point_sec voting_period )
{
    // no authorization required (can be executed by any account)

    // settings
    auto state = _state.get();
    auto settings = _settings.get();
}

void wps::check_completed()
{
    check( current_time_point() < _state.get().next_voting_period, "current voting period phase is now completed, any account must now execute [complete] ACTION to continue");
}