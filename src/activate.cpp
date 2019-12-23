// @action
void wps::activate( const eosio::name proposer, const eosio::name proposal_name, const eosio::time_point_sec voting_period )
{
    require_auth( proposer );
    const eosio::name ram_payer = get_self();

    // settings
    auto settings = _settings.get();
    auto state = _state.get();

    // voting period must be current or next
    check( voting_period == state.current_voting_period || voting_period == state.next_voting_period, "[voting_period] must equal to [current_voting_period] or [next_voting_period]");

    // cannot activate within 24 hours of next voting period ending
    const time_point end_voting_period = time_point( voting_period ) + time_point_sec(settings.voting_interval);
    check( current_time_point() + time_point_sec( DAY ) < end_voting_period, "cannot activate within 24 hours of next voting period ending");

    // cannot activate during completed voting period phase
    check_completed();

    // get scoped draft
    drafts_table _drafts( get_self(), proposer.value );
    auto drafts_itr = _drafts.find( proposal_name.value );
    auto proposals_itr = _proposals.find( proposal_name.value );

    // handle proposals that already exists
    if ( drafts_itr == _drafts.end() ) {
        check( proposals_itr == _proposals.end(), "[proposal_name] is already activated");
        check( false, "[proposal_name] draft does not exists");
    }
    check( proposals_itr == _proposals.end(), "[proposal_name] unfortunately already exists, please `canceldraft` and use a different proposal name");

    // get user deposit balance & substract amount
    auto deposits_itr = _deposits.find( proposer.value );
    check( deposits_itr->balance >= settings.deposit_required, "proposer's deposit balance does not meet minimum required amount of " + settings.deposit_required.to_string());
    sub_deposit( proposer, settings.deposit_required );
    move_to_locked_deposits( settings.deposit_required );

    // duration of proposal
    const time_point end = time_point(voting_period) + time_point_sec(settings.voting_interval * drafts_itr->duration);

    // convert draft proposal to active
    _proposals.emplace( ram_payer, [&]( auto& row ) {
        row.proposer        = proposer;
        row.proposal_name   = proposal_name;
        // inherit from draft
        row.title           = drafts_itr->title;
        row.monthly_budget  = drafts_itr->monthly_budget;
        row.duration        = drafts_itr->duration;
        row.total_budget    = drafts_itr->total_budget;
        row.proposal_json   = drafts_itr->proposal_json;
        // extras
        row.status          = "active"_n;
        row.total_net_votes = 0;
        row.payments        = asset{0, symbol{"EOS", 4}};
        row.created         = current_time_point();
        row.start           = voting_period;
        row.end             = end;
    });

    // erase draft
    _drafts.erase( drafts_itr );

    // add empty votes for proposal
    _votes.emplace( ram_payer, [&]( auto& row ) {
        row.proposal_name = proposal_name;
    });

    // add proposal name to time periods
    proposal_to_periods( proposal_name, voting_period, drafts_itr->duration, ram_payer );
}
