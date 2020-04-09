[[eosio::action]]
void wps::activate( const name proposer, const name proposal_name, bool activate_next )
{
    require_auth( proposer );

    // is contract paused or not
    check_contract_active();

    // check if proposer is eligible to activate proposal
    check_eligible_proposer( proposer );

    // check if account has enough funding
    // cannot activate proposals during insolvent voting period
    check_available_funding();

    // check if less than 100 active proposals
    // prevents excessive execute time for processing large amounts of proposals
    check_max_number_proposals();

    // cannot activate during completed voting period phase
    check_voting_period_completed();

    // minimum time required to activate at the end of the current voting period
    if ( !activate_next ) check_min_time_voting_end( );

    // can't create proposal that already exists
    check_draft_proposal_exists( proposer, proposal_name );

    // pay activation fee
    deduct_proposal_activate_fee( proposer );

    // create proposal
    emplace_proposal_from_draft( proposer, proposal_name, activate_next );

    // add empty votes for proposal
    emplace_empty_votes( proposal_name );

    // add proposal name to time periods
    if ( !activate_next ) add_proposal_to_periods( proposal_name );
}

void wps::check_min_time_voting_end()
{
    auto state = _state.get();
    auto settings = _settings.get();
    const time_point end_voting_period = time_point( state.current_voting_period ) + time_point_sec( settings.voting_interval );
    check( current_time_point() + time_point_sec( settings.min_time_voting_end ) < end_voting_period, "cannot activate within " + to_string( settings.min_time_voting_end ) + " seconds of next voting period ending");
}

void wps::check_draft_proposal_exists( const name proposer, const name proposal_name )
{
    // get scoped draft
    drafts_table _drafts( get_self(), proposer.value );
    auto drafts_itr = _drafts.find( proposal_name.value );
    auto proposals_itr = _proposals.find( proposal_name.value );

    // handle proposals that already exists
    if ( drafts_itr == _drafts.end() ) {
        check( proposals_itr == _proposals.end(), "[proposal_name] is already activated");
        check( false, "[proposal_name] draft does not exist");
    }
    check( proposals_itr == _proposals.end(), "[proposal_name] unfortunately already exists, please `canceldraft` and use a different proposal name");
}

void wps::deduct_proposal_activate_fee( const name proposer )
{
    auto settings = _settings.get();

    // get user deposit balance & substract amount
    auto deposits_itr = _deposits.find( proposer.value );
    check( deposits_itr->balance >= settings.deposit_required, "proposer's deposit balance does not meet minimum required amount of " + settings.deposit_required.to_string());
    sub_deposit( proposer, settings.deposit_required );
    move_to_locked_deposits( settings.deposit_required );
}

void wps::emplace_proposal_from_draft( const name proposer, const name proposal_name, const bool activate_next )
{
    // settings
    const name ram_payer = get_self();
    auto settings = _settings.get();
    auto state = _state.get();
    drafts_table _drafts( get_self(), proposer.value );
    auto drafts_itr = _drafts.find( proposal_name.value );

    // convert draft proposal to active
    _proposals.emplace( ram_payer, [&]( auto& row ) {
        row.proposer            = proposer;
        row.proposal_name       = proposal_name;

        // inherit from draft
        row.title               = drafts_itr->title;
        row.monthly_budget      = drafts_itr->monthly_budget;
        row.duration            = drafts_itr->duration;
        row.total_budget        = drafts_itr->total_budget;
        row.proposal_json       = drafts_itr->proposal_json;

        // active/pending status
        row.status = activate_next ? "pending"_n : "active"_n;

        // extras
        row.total_net_votes             = 0;
        row.eligible                    = false;
        row.payouts                     = asset{0, symbol{"EOS", 4}};
        row.claimed                     = asset{0, symbol{"EOS", 4}};
        row.created                     = current_time_point();
        row.start_voting_period         = activate_next ? time_point_sec(0) : state.current_voting_period;
        row.remaining_voting_periods    = drafts_itr->duration;
    });

    // erase draft
    _drafts.erase( drafts_itr );
}

void wps::emplace_empty_votes( const name proposal_name )
{
    const name ram_payer = get_self();

    // empty votes for proposal
    auto votes_itr = _votes.find( proposal_name.value );

    check( votes_itr == _votes.end(), "[create_empty_votes] votes already exist");

    _votes.emplace( ram_payer, [&]( auto& row ) {
        row.proposal_name = proposal_name;
    });
}

void wps::check_eligible_proposer( const name proposer )
{
    eosiosystem::producers_table _producers( "eosio"_n, "eosio"_n.value );
    check( _producers.find( proposer.value ) == _producers.end(), "[proposer] cannot be a registered producer");
}
