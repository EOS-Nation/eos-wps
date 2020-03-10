[[eosio::action]]
void wps::setparams( const wps_parameters params )
{
    require_auth( get_self() );
    const name ram_payer = get_self();
    check( params.voting_interval >= DAY, "[voting_interval] must be equal or above 24 hours (86400)");
    check( params.deposit_required.symbol == CORE_SYMBOL, "[deposit_required] invalid CORE_SYMBOL");
    check( params.max_monthly_budget.symbol == CORE_SYMBOL, "[max_monthly_budget] invalid CORE_SYMBOL");

    _settings.set( params, ram_payer );
}

void wps::add_funding( const asset quantity )
{
    const name ram_payer = get_self();

    auto state = _state.get();
    state.available_funding += quantity;
    _state.set( state, ram_payer );
}

void wps::sub_funding( const asset quantity )
{
    const name ram_payer = get_self();

    auto state = _state.get();
    state.available_funding -= quantity;
    _state.set( state, ram_payer );
}

