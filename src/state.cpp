void wps::init( const eosio::time_point_sec initial_voting_period )
{
    require_auth( get_self() );

    if ( !TESTING ) check( !_state.exists(), "already initialized" );

    auto state = _state.get_or_default();
    auto settings = _settings.get_or_default();

    state.current_voting_period = initial_voting_period;
    state.liquid_deposits = asset{0, symbol{"EOS", 4}};
    state.locked_deposits = asset{0, symbol{"EOS", 4}};
    state.available_funding = asset{0, symbol{"EOS", 4}};

    _state.set( state, get_self() );
    _settings.set( settings, get_self() );
}