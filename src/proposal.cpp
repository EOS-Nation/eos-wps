namespace eosio {

void wps::propose(const eosio::name proposer,
                  const eosio::name proposal_name,
                  const string title,
                  const eosio::asset budget,
                  const uint8_t duration,
                  const std::map<name, string> proposal_json )
{
    require_auth( proposer );
    check( _settings.exists(), "settings must first be initialized");
    auto settings = _settings.get_or_default();

    // validation
    check( _proposals.find( proposal_name.value ) == _proposals.end(), "[proposal_name] already exists" );
    check( proposal_name.length() > 2, "[proposal_name] should be at least 3 characters long" );
    check( title.size() < 1024, "[title] should be less than 1024 characters long" );
    check( budget.symbol == symbol{"EOS", 4}, "[budget] must use EOS symbol" );
    check( budget.amount >= 1000000, "[budget] must be a minimum of 100.0000 EOS ");
    check( duration > 0, "[duration] must be a minimum of 1 monthly period" );
    check( duration <= 6, "[duration] must not exceed 6 monthly periods" );

    // add row
    _proposals.emplace( proposer, [&]( auto& row ) {
        row.proposer = proposer;
        row.proposal_name = proposal_name;
        row.title = title;
        row.budget = budget;
        row.duration = duration;
        row.status = "draft"_n;
        row.deposit = asset{0, symbol{"EOS", 4}};
        row.payments = asset{0, symbol{"EOS", 4}};
        row.proposal_json = proposal_json;
    });
}

void wps::activate( const eosio::name proposer, const eosio::name proposal_name )
{
    require_auth( proposer );
    auto settings = _settings.get_or_default();
    auto proposals_itr = _proposals.find( proposal_name.value );

    check( proposals_itr != _proposals.end(), "[proposal_name] does not exists");
    check( proposals_itr->status == "draft"_n, "proposal `status` must be in `draft`");
    check( proposals_itr->deposit >= settings.deposit_required, "deposit does not meet minimum required amount of " + settings.deposit_required.to_string());

    // cannot activate within 7 days of next voting period ending
    const time_point in_one_week = current_time_point() + time_point_sec(604800);
    const time_point end_voting_period = time_point(settings.current_voting_period) + time_point_sec(settings.voting_interval);
    check( in_one_week < end_voting_period, "cannot activate within 7 days of next voting period ending");

    // set proposal as active
    _proposals.modify( proposals_itr, proposer, [&]( auto& row ) {
        row.status = "active"_n;
    });

    // duration of proposal
    const time_point end = time_point(settings.current_voting_period) + time_point_sec(settings.voting_interval * proposals_itr->duration);

    // add empty votes for proposal
    _votes.emplace( proposer, [&]( auto& row ) {
        row.proposal_name = proposal_name;
        row.status = "active"_n;
        row.voting_period = settings.current_voting_period;
        row.start = current_time_point();
        row.end = end;
    });
}

void wps::refund( const eosio::name proposer, const eosio::name proposal_name )
{
    require_auth( proposer );

    auto proposals_itr = _proposals.find( proposal_name.value );

    check( proposals_itr != _proposals.end(), "[proposal_name] does not exists");
    check( proposals_itr->status == "draft"_n, "`refund` is only available for proposals in draft status");
    check( proposals_itr->deposit.amount > 0, "proposal has no more deposit to refund");

    // send liquid token
    token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
    transfer.send( get_self(), proposer, proposals_itr->deposit, "refund deposit" );

    // add deposit
    _proposals.modify( proposals_itr, same_payer, [&]( auto& row ) {
        row.deposit -= proposals_itr->deposit;
    });
}

void wps::canceldraft( const eosio::name proposer, const eosio::name proposal_name )
{
    require_auth( proposer );

    auto proposals_itr = _proposals.find( proposal_name.value );

    check( proposals_itr != _proposals.end(), "[proposal_name] does not exists");
    check( proposals_itr->status == "draft"_n, "`canceldraft` is only available for proposals in draft status");
    check( proposals_itr->deposit.amount == 0, "must `refund` remaining balance before closing");

    // add deposit
    _proposals.erase( proposals_itr );
}

}