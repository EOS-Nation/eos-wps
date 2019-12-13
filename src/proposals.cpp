// @action
void wps::activate( const eosio::name proposer, const eosio::name proposal_name )
{
    require_auth( proposer );
    auto settings = _settings.get_or_default();
    auto state = _state.get_or_default();

    // get scoped draft
    drafts_table _drafts( get_self(), proposer.value );
    auto drafts_itr = _drafts.find( proposal_name.value );
    check( drafts_itr != _drafts.end(), "[proposal_name] draft does not exists");

    // get user deposit balance & substract amount
    auto deposits_itr = _deposits.find( proposer.value );
    check( deposits_itr->balance >= settings.deposit_required, "proposer's deposit balance does not meet minimum required amount of " + settings.deposit_required.to_string());
    sub_deposit( proposer, settings.deposit_required );
    move_to_locked_deposits( settings.deposit_required );

    // make sure proposal name is unique
    auto proposals_itr = _proposals.find( proposal_name.value );
    check( proposals_itr == _proposals.end(), "[proposal_name] unfortunately already exists, please `canceldraft` and try a using a new proposal_name");

    // cannot activate within 24 hours of next voting period ending
    const time_point end_voting_period = time_point(state.current_voting_period) + time_point_sec(settings.voting_interval);
    check( current_time_point() + time_point_sec( DAY ) < end_voting_period, "cannot activate within 24 hours of next voting period ending");

    // duration of proposal
    const time_point end = time_point(state.current_voting_period) + time_point_sec(settings.voting_interval * drafts_itr->duration);

    // convert draft proposal to active
    _proposals.emplace( proposer, [&]( auto& row ) {
        row.proposer        = proposer;
        row.proposal_name   = proposal_name;
        row.title           = drafts_itr->title;
        row.monthly_budget  = drafts_itr->monthly_budget;
        row.duration        = drafts_itr->duration;
        row.total_budget    = drafts_itr->total_budget;
        row.proposal_json   = drafts_itr->proposal_json;
        row.created         = current_time_point();
        row.start           = state.current_voting_period;
        row.end             = end;
    });

    // erase draft
    _drafts.erase( drafts_itr );

    // add empty votes for proposal
    _votes.emplace( proposer, [&]( auto& row ) {
        row.proposal_name = proposal_name;
    });

    // add proposal name to time periods
    proposal_to_periods( proposal_name, drafts_itr->duration, proposer );
}
