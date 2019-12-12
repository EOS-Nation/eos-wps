// @action
void wps::setproposer(const eosio::name proposer,
                      const std::map<name, string> metadata_json )
{
    require_auth( proposer );

    auto proposers_itr = _proposers.find( proposer.value );

    if ( proposers_itr == _proposers.end() ) {
        _proposers.emplace( proposer, [&]( auto& row ) {
            row.proposer        = proposer;
            row.metadata_json   = metadata_json;
        });
    } else {
        _proposers.modify( proposers_itr, same_payer, [&]( auto& row ) {
            row.metadata_json   = metadata_json;
        });
    }
}
