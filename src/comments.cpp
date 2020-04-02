[[eosio::action]]
void wps::comment( const name account, const name proposal_name, const map<name, string> metadata_json )
{
    require_auth( account );
    const eosio::name ram_payer = account;

    comments_table _comments( get_self(), proposal_name.value );
    auto comments_itr = _comments.find( account.value );
    const auto proposal = _proposals.get( proposal_name.value, "[proposal_name] does not exists" );

    // only proposer and/or eligile voters are allowed to comment
    const map<name, name> votes = _votes.get( proposal_name.value, "[proposal_name] does not exists" ).votes;
    const bool is_voting = votes.find( account ) != votes.end();
    check( proposal.proposer == account || is_voting, "[account] not eligible to comment on [proposal_name], must be proposer or voted on proposal");

    if ( comments_itr == _comments.end() ) {
        check( metadata_json.size(), "[metadata_json] is empty");

        _comments.emplace( ram_payer, [&]( auto& row ) {
            row.account = account;
            row.timestamp = current_time_point();
            row.version = 0;
            row.metadata_json = metadata_json;
        });
    } else {
        check( proposal.status == "active"_n, "cannot modify/erase comment when proposal is no longer active");

        // modify comment
        if ( metadata_json.size() ) {
            _comments.modify( comments_itr, same_payer, [&]( auto& row ) {
                row.timestamp = current_time_point();
                row.version += 1;
                row.metadata_json = metadata_json;
            });
        // delete comment
        } else {
            _comments.erase( comments_itr );
        }
    }
}
