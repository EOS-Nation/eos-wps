// @action
void wps::submitdraft(const eosio::name proposer,
                      const eosio::name proposal_name,
                      const string title,
                      const eosio::asset budget,
                      const uint8_t duration,
                      const std::map<name, string> proposal_json )
{
    require_auth( proposer );
    check( _settings.exists(), "settings must first be initialized");

    auto settings = _settings.get_or_default();

    // get scoped draft
    drafts_table _drafts( get_self(), proposer.value );
    auto drafts_itr = _drafts.find( proposal_name.value );
    auto proposals_itr = _proposals.find( proposal_name.value );

    // validation
    check( proposals_itr == _proposals.end(), "[proposal_name] activated proposal already exists, try using a different proposal name" );
    check( drafts_itr == _drafts.end(), "[proposal_name] draft already exists, try using `modifydraft` or `canceldraft`" );
    check( proposal_name.length() > 2, "[proposal_name] should be at least 3 characters in length" );
    check( proposal_name.length() < 13, "[proposal_name] cannot exceed 12 characters in length" );
    check_title( title );
    check( budget.symbol == symbol{"EOS", 4}, "[budget] must use EOS symbol" );
    check( budget >= settings.deposit_required, "[budget] must be a minimum of " + settings.deposit_required.to_string());
    check( duration > 0, "[duration] must be a minimum of 1 monthly period" );
    check( duration <= 6, "[duration] must not exceed 6 monthly periods" );

    // create draft proposal
    _drafts.emplace( proposer, [&]( auto& row ) {
        row.proposer        = proposer;
        row.proposal_name   = proposal_name;
        row.title           = title;
        row.budget          = budget;
        row.duration        = duration;
        row.proposal_json   = proposal_json;
    });

    create_deposit_account( proposer );
}

// @action
void wps::modifydraft(const eosio::name proposer,
                      const eosio::name proposal_name,
                      const string title,
                      const std::map<name, string> proposal_json )
{
    require_auth( proposer );

    // get scoped draft
    drafts_table _drafts( get_self(), proposer.value );
    auto drafts_itr = _drafts.find( proposal_name.value );

    check( drafts_itr != _drafts.end(), "[proposal_name] draft does not exists");
    check_title( title );

    // modify row
    _drafts.modify( drafts_itr, same_payer, [&]( auto& row ) {
        row.title = title;
        row.proposal_json = proposal_json;
    });
}

// @action
void wps::canceldraft( const eosio::name proposer, const eosio::name proposal_name )
{
    require_auth( proposer );

    // get scoped drafts
    drafts_table _drafts( get_self(), proposer.value );
    auto drafts_itr = _drafts.find( proposal_name.value );

    check( drafts_itr != _drafts.end(), "[proposal_name] draft does not exists");

    // add deposit
    _drafts.erase( drafts_itr );
}

void wps::check_title( const string title )
{
    check( title.size() < 1024, "[title] should be less than 1024 characters long" );
    check( title.size() > 2, "[title] should be at least 3 characters long" );
}