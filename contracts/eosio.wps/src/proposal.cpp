namespace eosio {

void wps::propose(const eosio::name proposer,
                  const eosio::name proposal_name,
                  const string title,
                  const string proposal_json,
                  const eosio::asset budget,
                  const uint8_t payments )
{
    require_auth( proposer );
    proposal_table _proposal( get_self(), get_self().value );

    // validation
    check( _proposal.find( proposal_name.value ) == _proposal.end(), "[proposal_name] already exists" );
    check( proposal_name.length() > 2, "[proposal_name] should be at least 3 characters long" );
    check( title.size() < 1024, "[title] should be less than 1024 characters long" );
    check_json( proposal_json );
    check( budget.symbol == symbol{"EOS", 4}, "[budget] must use EOS symbol" );
    check( budget.amount >= 1000000, "[budget] must be a minimum of 100.0000 EOS ");
    check( payments > 0, "[payments] must be a minimum of 1 monthly period" );
    check( payments <= 6, "[payments] must not exceed 6 monthly periods" );

    // add row
    _proposal.emplace( proposer, [&]( auto& row ) {
        row.proposer = proposer;
        row.proposal_name = proposal_name;
        row.title = title;
        row.proposal_json = proposal_json;
        row.budget = budget;
        row.payments = payments;
        row.status = "draft"_n;
    });
}

void wps::check_json( const string json )
{
    if (json.size() <= 0) return;

    check(json[0] == '{' && json[json.size() - 1] == '}', "must be a JSON object (if specified)" );
    check( sizeof(json) < 32768, "JSON should be shorter than 32768 bytes" );
}

}