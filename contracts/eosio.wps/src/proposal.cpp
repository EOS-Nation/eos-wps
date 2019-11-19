namespace eosio {

void wps::propose(const eosio::name proposer,
                  const eosio::name proposal_name,
                  const string title,
                  const string proposal_json,
                  const eosio::asset budget,
                  const uint8_t payments )
{
    require_auth( proposer );
    check( _settings.exists(), "settings must first be initialized");
    auto settings = _settings.get_or_default();

    // validation
    check( _proposals.find( proposal_name.value ) == _proposals.end(), "[proposal_name] already exists" );
    check( proposal_name.length() > 2, "[proposal_name] should be at least 3 characters long" );
    check( title.size() < 1024, "[title] should be less than 1024 characters long" );
    check_json( proposal_json );
    check( budget.symbol == symbol{"EOS", 4}, "[budget] must use EOS symbol" );
    check( budget.amount >= 1000000, "[budget] must be a minimum of 100.0000 EOS ");
    check( payments > 0, "[payments] must be a minimum of 1 monthly period" );
    check( payments <= 6, "[payments] must not exceed 6 monthly periods" );

    // add row
    _proposals.emplace( proposer, [&]( auto& row ) {
        row.proposer = proposer;
        row.proposal_name = proposal_name;
        row.title = title;
        row.proposal_json = proposal_json;
        row.budget = budget;
        row.payments = payments;
        row.deposit = asset{0, symbol{"EOS", 4}};
        row.status = "draft"_n;
        row.start = time_point_sec(0);
        row.end = time_point_sec(0);
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

    // duration of proposal
    const time_point end = time_point(settings.current_voting_period) + time_point_sec(settings.voting_interval * proposals_itr->payments);

    _proposals.modify( proposals_itr, proposer, [&]( auto& row ) {
        row.status = "active"_n;
        row.start = current_time_point();
        row.end = end;
    });
}

void wps::withdraw( const eosio::name proposer, const eosio::name proposal_name )
{
    require_auth( proposer );

    auto proposals_itr = _proposals.find( proposal_name.value );

    check( proposals_itr != _proposals.end(), "[proposal_name] does not exists");
    check( proposals_itr->status == "draft"_n, "withdraw is only available for proposals in draft status");
    check( proposals_itr->deposit.amount > 0, "proposal has no more deposit to withdraw");

    // send liquid token
    token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
    transfer.send( get_self(), proposer, proposals_itr->deposit, "withdraw deposit" );

    // add deposit
    _proposals.modify( proposals_itr, same_payer, [&]( auto& row ) {
        row.deposit -= proposals_itr->deposit;
    });
}

void wps::check_json( const string json )
{
    if (json.size() <= 0) return;

    check(json[0] == '{' && json[json.size() - 1] == '}', "must be a JSON object (if specified)" );
    check( sizeof(json) < 32768, "JSON should be shorter than 32768 bytes" );
}

}