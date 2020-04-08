[[eosio::action]]
void wps::init( const wps_parameters params )
{
    require_auth( get_self() );
    const name ram_payer = get_self();

    check( !_state.exists(), "already initialized" );
    check_wps_parameters( params );

    // define `settings`
    auto settings = params;
    _settings.set( settings, ram_payer );

    // set available BUDGET_SYMBOL ("EOS") as `available_funding`
    auto state = _state.get_or_default();
    state.available_funding = token::get_balance( BUDGET_TOKEN_CONTRACT, get_self(), BUDGET_SYMBOL.code() );

    // // UNCOMMENT FOR PRODUCTION
    // // start of voting period will start at the nearest 00:00UTC
    // const uint64_t now = current_time_point().sec_since_epoch();
    // const time_point_sec current_voting_period = time_point_sec(now - now % DAY);

    if ( TESTING ) print("set `current_voting_period` to now");
    const time_point_sec current_voting_period = current_time_point();

    // define `state`
    state.current_voting_period = current_voting_period;
    state.next_voting_period = state.current_voting_period + settings.voting_interval;
    _state.set( state, ram_payer );

    // check if WPS account has enough funding to initialize the first voting period
    check_available_funding();
}

[[eosio::action]]
void wps::setparams( const wps_parameters params )
{
    require_auth( get_self() );
    check_wps_parameters( params );

    _settings.set( params, get_self() );
}

void wps::check_wps_parameters( const wps_parameters params )
{
    if ( TESTING ) print("removed `voting_interval` check");
    // // UNCOMMENT FOR PRODUCTION
    // check( params.voting_interval == MONTH, "[voting_interval] must equal to 30 days (2592000)");
    check( params.deposit_required.symbol == BUDGET_SYMBOL, "[deposit_required] invalid symbol");
    check( params.max_monthly_budget.symbol == BUDGET_SYMBOL, "[max_monthly_budget] invalid symbol");
    check( params.deposit_required.amount >= 0, "[deposit_required] must be positive");
    check( params.max_monthly_budget.amount >= 0, "[max_monthly_budget] must be positive");
    check( params.vote_margin > 0, "[vote_margin] must be above zero");
    check( params.min_time_voting_end < params.voting_interval, "[min_time_voting_end] must less than [voting_interval]");
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

    const asset balance = token::get_balance( BUDGET_TOKEN_CONTRACT, get_self(), BUDGET_SYMBOL.code() );
    check( state.available_funding >= settings.max_monthly_budget, "[available_funding] must be equal or greater than [max_monthly_budget]");
    check( balance >= settings.max_monthly_budget, "[balance] must be equal or greater than [max_monthly_budget]");
}