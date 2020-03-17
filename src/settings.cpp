[[eosio::action]]
void wps::init( const wps_parameters params )
{
    require_auth( get_self() );
    const name ram_payer = get_self();

    check( !_state.exists(), "already initialized" );

    // define `settings`
    auto settings = params;
    _settings.set( settings, ram_payer );

    // set available EOS as `available_funding`
    auto state = _state.get_or_default();
    state.available_funding = token::get_balance( "eosio.token"_n, get_self(), CORE_SYMBOL.code() );

    // start of voting period will start at the nearest 00:00UTC
    const uint64_t now = current_time_point().sec_since_epoch();
    const time_point_sec current_voting_period = time_point_sec(now - now % DAY);

    // define `state`
    state.current_voting_period = current_voting_period;
    state.next_voting_period = state.current_voting_period + settings.voting_interval;
    _state.set( state, ram_payer );

    // check if account has enough funding
    check_available_funding();

    // must manually execute the `start` action after `init` to begin voting period
}

[[eosio::action]]
void wps::setparams( const wps_parameters params )
{
    require_auth( get_self() );

    check( params.voting_interval >= DAY, "[voting_interval] must be equal or above 24 hours (86400)");
    check( params.deposit_required.symbol == CORE_SYMBOL, "[deposit_required] invalid CORE_SYMBOL");
    check( params.max_monthly_budget.symbol == CORE_SYMBOL, "[max_monthly_budget] invalid CORE_SYMBOL");

    _settings.set( params, get_self() );
}

void wps::check_contract_active()
{
    check( _state.exists(), "contract not yet initialized" );
    check( _settings.exists(), "settings are missing" );
}

void wps::add_funding( const asset quantity )
{
    auto state = _state.get();
    state.available_funding += quantity;
    _state.set( state, get_self() );
}

void wps::sub_funding( const asset quantity )
{
    auto state = _state.get();
    state.available_funding -= quantity;
    _state.set( state, get_self() );
}

void wps::check_available_funding()
{
    auto state = _state.get();
    auto settings = _settings.get();

    const asset balance = token::get_balance( "eosio.token"_n, get_self(), CORE_SYMBOL.code() );
    check( state.available_funding >= settings.max_monthly_budget, "[available_funding] must be equal or greater than [max_monthly_budget]");
    check( balance >= settings.max_monthly_budget, "[balance] must be equal or greater than [max_monthly_budget]");
}