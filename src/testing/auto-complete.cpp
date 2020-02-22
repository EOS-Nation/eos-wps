void wps::auto_complete()
{
    // settings
    auto state = _state.get();

    // calculate remaining seconds
    int64_t delay_sec = ( state.next_voting_period - current_time_point() ).to_seconds();
    if ( delay_sec <= 0 ) delay_sec = 0;

    // in case current time is still in the future
    if ( current_time_point() >= time_point( state.next_voting_period ) ) delay_sec = 0;

    // send deferred
    uint64_t key = "complete"_n.value;
    wps::complete_action complete( get_self(), { get_self(), "active"_n });
    send_deferred( complete.to_action( ), key, delay_sec );
}