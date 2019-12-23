// @action
void wps::complete( const eosio::time_point_sec voting_period )
{
    // no authorization required (can be executed by any account)
    const eosio::name ram_payer = get_self();

    // settings
    auto state = _state.get();
    auto settings = _settings.get();

    // check if current voting period is completed
    check( state.current_voting_period == voting_period, "[voting_period] must equal to [current_voting_period]");
    check( current_time_point() >= time_point( state.next_voting_period ), "[current_voting_period] is not completed");

    // TO-DO
    // HANDLE PAYMENTS TO PROPOSALS

    // update current & next voting period
    state.current_voting_period = state.next_voting_period;
    state.next_voting_period = state.next_voting_period + settings.voting_interval;
    _state.set( state, ram_payer );

    // push deferred transaction using `next_voting_period` as the delay
    auto_complete();
}

void wps::auto_complete()
{
    // settings
    auto state = _state.get();

    // calculate remaining seconds
    int64_t delay_sec = ( state.next_voting_period - current_time_point() ).to_seconds();
    if ( delay_sec <= 0 ) delay_sec = 0;

    // send deferred
    uint64_t key = "complete"_n.value;
    wps::complete_action complete( get_self(), { get_self(), "active"_n });
    send_deferred( complete.to_action( state.next_voting_period ), key, delay_sec );
}

void wps::check_completed()
{
    // settings
    auto state = _state.get();

    check( current_time_point() < time_point( state.next_voting_period ), "[current_voting_period] is completed, any account must execute [complete] ACTION to continue");
}