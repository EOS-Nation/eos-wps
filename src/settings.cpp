// @action
void wps::init( const eosio::time_point_sec initial_voting_period )
{
    require_auth( get_self() );
    const eosio::name ram_payer = get_self();

    check( !_state.exists(), "already initialized" );

    auto state = _state.get_or_default();
    auto settings = _settings.get_or_default();

    state.current_voting_period = initial_voting_period;
    state.next_voting_period = initial_voting_period + settings.voting_interval;
    state.liquid_deposits = asset{0, symbol{"EOS", 4}};
    state.locked_deposits = asset{0, symbol{"EOS", 4}};
    state.available_funding = asset{0, symbol{"EOS", 4}};

    _state.set( state, ram_payer );
    _settings.set( settings, ram_payer );
}

// @action
void wps::setparams( const wps_parameters params )
{
    require_auth( get_self() );
    const eosio::name ram_payer = get_self();

    _settings.set( params, ram_payer );
}

void wps::add_funding( const eosio::asset quantity )
{
    const eosio::name ram_payer = get_self();

    auto state = _state.get_or_default();
    state.available_funding += quantity;
    _state.set( state, ram_payer );
}

void wps::sub_funding( const eosio::asset quantity )
{
    const eosio::name ram_payer = get_self();

    auto state = _state.get_or_default();
    state.available_funding -= quantity;
    _state.set( state, ram_payer );
}
