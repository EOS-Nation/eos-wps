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
    const time_point in_one_week = current_time_point() + time_point_sec( DAY );
    const time_point end_voting_period = time_point(state.current_voting_period) + time_point_sec(settings.voting_interval);
    check( in_one_week < end_voting_period, "cannot activate within 24 hours of next voting period ending");

    // convert draft proposal to active
    _proposals.emplace( proposer, [&]( auto& row ) {
        row.proposer        = drafts_itr->proposer;
        row.proposal_name   = drafts_itr->proposal_name;
        row.title           = drafts_itr->title;
        row.budget          = drafts_itr->budget;
        row.duration        = drafts_itr->duration;
        row.proposal_json   = drafts_itr->proposal_json;
    });

    // erase draft
    _drafts.erase( drafts_itr );

    // duration of proposal
    const time_point end = time_point(state.current_voting_period) + time_point_sec(settings.voting_interval * proposals_itr->duration);

    // add empty votes for proposal
    _votes.emplace( proposer, [&]( auto& row ) {
        row.proposal_name = proposal_name;
        row.voting_period = state.current_voting_period;
        row.end = end;
    });
}