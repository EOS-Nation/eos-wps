namespace eosio {

void wps::setparams( const wps_parameters params )
{
    require_auth( get_self() );
    _settings.set( params, get_self() );
}
}