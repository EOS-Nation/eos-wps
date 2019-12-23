void wps::send_deferred( const eosio::action action, const uint64_t key, const uint64_t interval )
{
    eosio::transaction deferred;
    deferred.delay_sec = interval;
    deferred.expiration = current_time_point() + time_point_sec(3600);
    deferred.actions.emplace_back( action );

    // cancel & send
    eosio::cancel_deferred( key );
    deferred.send( key, get_self() );
}
