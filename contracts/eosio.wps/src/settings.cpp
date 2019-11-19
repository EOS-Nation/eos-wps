void wps::init( const eosio::time_point_sec current_voting_period )
{
    require_auth( get_self() );

    auto settings = _settings.get_or_default();
    settings.current_voting_period = current_voting_period;
    _settings.set( settings, get_self() );
}

void wps::settings( const uint64_t vote_margin, const eosio::asset deposit_required, const uint64_t voting_interval )
{
    require_auth( get_self() );

    check( _settings.exists(), "settings must first be initialized");

    auto settings = _settings.get_or_default();
    settings.vote_margin = vote_margin;
    settings.deposit_required = deposit_required;
    settings.voting_interval = voting_interval;
    _settings.set( settings, get_self() );
}
