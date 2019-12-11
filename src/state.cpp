// @action
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

void wps::add_funding( const eosio::asset quantity )
{
    auto state = _state.get_or_default();
    state.available_funding += quantity;
    _state.set( state, get_self() );
}

void wps::add_liquid_deposits( const eosio::asset quantity )
{
    auto state = _state.get_or_default();
    state.liquid_deposits += quantity;
    _state.set( state, get_self() );
}

void wps::move_to_locked_deposits( const eosio::asset quantity )
{
    auto state = _state.get_or_default();
    state.liquid_deposits -= quantity;
    state.locked_deposits += quantity;
    _state.set( state, get_self() );
}