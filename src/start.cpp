[[eosio::action]]
void wps::init()
{
    require_auth( get_self() );
    const name ram_payer = get_self();

    check( !_state.exists(), "already initialized" );

    auto state = _state.get_or_default();
    auto settings = _settings.get_or_default();

    _state.set( state, ram_payer );
    _settings.set( settings, ram_payer );

    // must manually execute the `start` action to begin voting period
}

[[eosio::action]]
void wps::start()
{
    require_auth( get_self() );
    const name ram_payer = get_self();

    check( _state.exists(), "contract not yet initialized" );
    check( _settings.exists(), "settings are missing" );

    // TO-DO add checks

    auto state = _state.get();
    auto settings = _settings.get();


    // start of voting period will start at the nearest 00:00UTC
    const uint64_t now = current_time_point().sec_since_epoch();
    const time_point_sec current_voting_period = time_point_sec(now - now % DAY);

    // validation
    check( state.next_voting_period <= current_time_point(), "[next_voting_period] must be in the past");
    check( state.current_voting_period != current_voting_period, "[current_voting_period] was not modified");

    state.current_voting_period = current_voting_period;
    state.next_voting_period = state.current_voting_period + settings.voting_interval;
    _state.set( state, ram_payer );

    // must manually execute the `complete` action to finish voting period
}