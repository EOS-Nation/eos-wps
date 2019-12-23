/**
 * TESTING ONLY
 *
 * Should be removed in production
 */
void wps::setstate( const state_row params )
{
    require_auth( get_self() );
    const eosio::name ram_payer = get_self();

    _state.set( params, ram_payer );
}
