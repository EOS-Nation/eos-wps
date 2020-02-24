[[eosio::action]]
void wps::activate( const eosio::name proposer, const eosio::name proposal_name, std::optional<eosio::time_point_sec> start_voting_period )
{
    require_auth( proposer );
    const eosio::name ram_payer = get_self();

    // TESTING PURPOSES
    eosio::time_point_sec voting_period = time_point_sec( start_voting_period->sec_since_epoch() );

    // set start voting period to current voting period if defined as null
    if ( start_voting_period->sec_since_epoch() == 0 ) voting_period = _state.get().current_voting_period;

    // voting period must be current or next
    check_start_vote_period( voting_period );

    // minimum time required to activate at the end of the current voting period
    check_min_time_voting_end( voting_period );

    // cannot activate during completed voting period phase
    check_voting_period_completed();

    // can't create proposal that already exists
    check_draft_proposal_exists( proposer, proposal_name );

    // pay activation fee
    deduct_proposal_activate_fee( proposer );

    // create proposal
    emplace_proposal_from_draft( proposer, proposal_name, voting_period, ram_payer );

    // add empty votes for proposal
    emplace_empty_votes( proposal_name, ram_payer );

    // add proposal name to time periods
    proposal_to_periods( proposal_name, ram_payer );
}

void wps::check_min_time_voting_end( const eosio::time_point_sec start_voting_period )
{
    auto settings = _settings.get();
    const time_point end_voting_period = time_point( start_voting_period ) + time_point_sec(settings.voting_interval);
    check( current_time_point() + time_point_sec( settings.min_time_voting_end ) < end_voting_period, "cannot activate within " + to_string(settings.min_time_voting_end) + " seconds of next voting period ending");
}

void wps::proposal_to_periods( const eosio::name proposal_name, const eosio::name ram_payer )
{
    // settings
    auto settings = _settings.get();
    auto proposals_itr = _proposals.find( proposal_name.value );

    // insert proposal name into multiple periods
    for (int i = 0; i < proposals_itr->duration; i++) {
        const eosio::time_point_sec voting_period = time_point(proposals_itr->start_voting_period) + time_point_sec(settings.voting_interval * i);
        auto periods_itr = _periods.find( voting_period.sec_since_epoch() );

        // create new set of proposals
        if ( periods_itr == _periods.end() ) {
            _periods.emplace( ram_payer, [&]( auto& row ) {
                row.voting_period   = voting_period;
                row.proposals       = std::set<eosio::name> { proposal_name };
            });
        // insert proposal to old ones
        } else {
            _periods.modify( periods_itr, ram_payer, [&]( auto& row ) {
                row.proposals.insert( proposal_name );
                check(row.proposals.size() <= 100, "cannot exceed 100 proposals per single voting period");
            });
        }
    }
}

void wps::check_draft_proposal_exists( const eosio::name proposer, const eosio::name proposal_name )
{
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
}

void wps::deduct_proposal_activate_fee( const eosio::name proposer )
{
    auto settings = _settings.get();

    // get user deposit balance & substract amount
    auto deposits_itr = _deposits.find( proposer.value );
    check( deposits_itr->balance >= settings.deposit_required, "proposer's deposit balance does not meet minimum required amount of " + settings.deposit_required.to_string());
    sub_deposit( proposer, settings.deposit_required );
    move_to_locked_deposits( settings.deposit_required );
}

void wps::emplace_proposal_from_draft( const eosio::name proposer, const eosio::name proposal_name, const eosio::time_point_sec start_voting_period, const eosio::name ram_payer )
{
    // settings
    auto settings = _settings.get();
    auto state = _state.get();
    drafts_table _drafts( get_self(), proposer.value );
    auto drafts_itr = _drafts.find( proposal_name.value );

    // duration of proposal
    const time_point end = time_point(start_voting_period) + time_point_sec(settings.voting_interval * drafts_itr->duration);

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
        if ( start_voting_period == time_point(state.current_voting_period) ) row.status = "active"_n;
        else row.status = "pending"_n;

        // extras
        row.total_net_votes     = 0;
        row.eligible            = false;
        row.payouts             = asset{0, symbol{"EOS", 4}};
        row.claimed             = asset{0, symbol{"EOS", 4}};
        row.created             = current_time_point();
        row.start_voting_period = start_voting_period;
        row.end                 = end;
    });

    // erase draft
    _drafts.erase( drafts_itr );
}

void wps::emplace_empty_votes( const eosio::name proposal_name, const eosio::name ram_payer )
{
    // empty votes for proposal
    auto votes_itr = _votes.find( proposal_name.value );

    check( votes_itr == _votes.end(), "[create_empty_votes] votes already exists");

    _votes.emplace( ram_payer, [&]( auto& row ) {
        row.proposal_name = proposal_name;
    });
}

void wps::check_start_vote_period( const eosio::time_point_sec start_voting_period )
{
    auto state = _state.get();
    check( start_voting_period == state.current_voting_period || start_voting_period == state.next_voting_period, "[start_voting_period] must equal to [current_voting_period] or [next_voting_period]");
}
