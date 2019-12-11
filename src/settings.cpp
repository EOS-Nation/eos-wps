void wps::init( const eosio::time_point_sec initial_voting_period )
{
    require_auth( get_self() );

    if ( !TESTING ) check( !_current.exists(), "already initialized" );

    auto current = _current.get_or_default();
    auto settings = _settings.get_or_default();

    current.voting_period = initial_voting_period;
    _current.set( current, get_self() );
    _settings.set( settings, get_self() );
}

void wps::setsettings( const uint64_t vote_margin, const eosio::asset deposit_required, const uint64_t voting_interval )
{
    require_auth( get_self() );

    auto settings = _settings.get_or_default();
    settings.vote_margin = vote_margin;
    settings.deposit_required = deposit_required;
    settings.voting_interval = voting_interval;
    _settings.set( settings, get_self() );
}
