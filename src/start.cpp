[[eosio::action]]
void wps::init()
{
    require_auth( get_self() );
    const name ram_payer = get_self();

    check( !_state.exists(), "already initialized" );

    auto state = _state.get_or_default();
    auto settings = _settings.get_or_default();

    // open token balance if does not exists
    token::accounts _accounts( "eosio.token"_n, get_self().value );
    auto accounts_itr = _accounts.find( CORE_SYMBOL.code().raw() );

    // open token balance
    if ( accounts_itr == _accounts.end() ) {
        token::open_action open( "eosio.token"_n, { get_self(), "active"_n });
        open.send( get_self(), CORE_SYMBOL, get_self() );
    } else {
        // set any remaining EOS as `available_funding`
        state.available_funding = token::get_balance( "eosio.token"_n, get_self(), CORE_SYMBOL.code() );
    }

    _state.set( state, ram_payer );
    _settings.set( settings, ram_payer );

    // must manually execute the `start` action after `init` to begin voting period
}

[[eosio::action]]
void wps::start()
{
    require_auth( get_self() );
    const name ram_payer = get_self();

    check( _state.exists(), "contract not yet initialized" );
    check( _settings.exists(), "settings are missing" );

    // check if account has enough funding
    check_available_funding();

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

void wps::check_available_funding()
{
    auto state = _state.get();
    auto settings = _settings.get();

    const asset balance = token::get_balance( "eosio.token"_n, get_self(), CORE_SYMBOL.code() );
    check( state.available_funding >= settings.max_monthly_budget, "[available_funding] must be equal or greater than [max_monthly_budget]");
    check( balance >= settings.max_monthly_budget, "[balance] must be equal or greater than [max_monthly_budget]");
}