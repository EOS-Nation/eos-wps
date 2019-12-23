// @action
void wps::complete( const eosio::time_point_sec voting_period )
{
    // no authorization required (can be executed by any account)

    // settings
    auto state = _state.get();
    auto settings = _settings.get();
}
