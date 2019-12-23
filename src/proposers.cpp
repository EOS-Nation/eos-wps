// @action
void wps::setproposer(const eosio::name proposer, const std::map<name, string> metadata_json )
{
    require_auth( proposer );
    const eosio::name ram_payer = proposer;

    auto proposers_itr = _proposers.find( proposer.value );

    if ( proposers_itr == _proposers.end() ) {
        _proposers.emplace( ram_payer, [&]( auto& row ) {
            row.proposer        = proposer;
            row.metadata_json   = metadata_json;
        });
    } else {
        _proposers.modify( proposers_itr, ram_payer, [&]( auto& row ) {
            row.metadata_json   = metadata_json;
        });
    }
}
