// @action
void wps::complete( )
{
    // no authorization required (can be executed by any account)

    // // check if current voting period is completed
    // check( is_voting_period_complete(), "[current_voting_period] is not completed");

    // payouts of active proposals
    handle_payouts();

    // set pending proposals to active status
    set_pending_to_active();

    // update current & next voting period
    update_to_next_voting_period();

    // push deferred transaction using `next_voting_period` as the delay
    auto_complete();
}

bool wps::is_voting_period_complete()
{
    return current_time_point() >= time_point( _state.get().next_voting_period );
}

void wps::update_to_next_voting_period()
{
    auto state = _state.get();
    auto settings = _settings.get();

    state.current_voting_period = state.next_voting_period;
    state.next_voting_period = state.next_voting_period + settings.voting_interval;
    _state.set( state, same_payer );
}

void wps::handle_payouts()
{
    // settings
    auto settings = _settings.get();
    auto state = _state.get();

    // static actions
    wps::claim_action claim( get_self(), { get_self(), "active"_n });

    // containers
    eosio::asset total_payout = asset{ 0, symbol{ "EOS", 4 }};

    // available EOS balance of wps contract
    eosio::asset wps_balance = token::get_balance( "eosio.token"_n, get_self(), symbol_code("EOS") );

    // filter out min voting threshold proposals
    std::map<int16_t, std::set<eosio::name>> proposals = sort_proposals_by_net_votes( "active"_n );

    // iterate proposals from highest to lowest net votes
    for ( auto itr = proposals.rbegin(); itr != proposals.rend(); ++itr ) {

        // iterate over proposals
        for ( auto proposal_name : itr->second ) {
            // proposal variables
            auto proposal_itr = _proposals.find( proposal_name.value );
            int16_t total_net_votes = itr->first;
            const eosio::asset monthly_budget = proposal_itr->monthly_budget;
            const eosio::name proposer = proposal_itr->proposer;

            // min requirements for payouts
            const bool is_min_vote_margin = total_net_votes >= settings.vote_margin;
            const bool is_enough_budget = (total_payout + monthly_budget) <= settings.max_monthly_budget;

            // payout following proposal
            if ( is_enough_budget && is_min_vote_margin ) {
                total_payout += monthly_budget;

                // check internal funding
                check( state.available_funding >= monthly_budget, "insufficient `available_funding`");
                sub_funding( monthly_budget );

                // check eosio.wps balance
                wps_balance -= monthly_budget;
                check( wps_balance.amount >= 0, "insufficient balance");

                // update proposal payouts
                _proposals.modify( proposal_itr, same_payer, [&]( auto& row ) {
                    row.payouts += monthly_budget;
                    row.claimable += monthly_budget;
                });

                // push 0 second deferred transaction to auto-execute claim
                send_deferred( claim.to_action( proposal_name ), proposal_name.value + "claim"_n.value, 0 );
            }
            // set payout as completed/expired/active based on payout received
            update_proposal_status( proposal_name );
        }
    }
}

std::map<int16_t, std::set<eosio::name>> wps::sort_proposals_by_net_votes( const eosio::name status )
{
    auto index = _proposals.get_index<"bystatus"_n>();

    // add proposals to ordered map
    std::map<int16_t, std::set<eosio::name>> proposals;

    for ( auto itr = index.lower_bound(status.value); itr != index.upper_bound(status.value); ++itr ) {
        proposals[itr->total_net_votes].insert(itr->proposal_name);
    }
    return proposals;
}

void wps::update_proposal_status( const eosio::name proposal_name )
{
    auto state = _state.get();
    auto proposal_itr = _proposals.find( proposal_name.value );

    _proposals.modify( proposal_itr, same_payer, [&]( auto& row ) {
        if ( !proposal_exists_per_voting_period( proposal_name, state.next_voting_period )) {
            if ( row.payouts == row.total_budget ) row.status = "completed"_n;
            else if ( row.payouts.amount > 0 ) row.status = "partial"_n;
            else row.status = "expired"_n;
        }
    });
}

bool wps::proposal_exists_per_voting_period( const eosio::name proposal_name, const eosio::time_point_sec voting_period )
{
    auto itr = _periods.find( voting_period.sec_since_epoch() );
    if ( itr == _periods.end()) return false;
    return itr->proposals.find( proposal_name ) != itr->proposals.end();
}

void wps::set_pending_to_active()
{
    auto index = _proposals.get_index<"bystatus"_n>();
    auto itr = index.find("pending"_n.value);

    if (itr == index.end()) return;

    index.modify( itr, same_payer, [&]( auto& row ) {
        row.status = "active"_n;
    });
}

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

void wps::check_voting_period_completed()
{
    auto state = _state.get();
    check( current_time_point() < time_point( state.next_voting_period ), "[current_voting_period] is completed, any account must execute [complete] ACTION to continue");
}