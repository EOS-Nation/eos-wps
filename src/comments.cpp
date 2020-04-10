[[eosio::action]]
void wps::comment( const name account, const name proposal_name, const map<name, string> comment_json )
{
    require_auth( account );
    const eosio::name ram_payer = account;

    comments_table _comments( get_self(), proposal_name.value );
    auto comments_itr = _comments.find( account.value );
    const auto proposal = _proposals.get( proposal_name.value, "[proposal_name] does not exists" );

    // find if account is voting for proposal
    const map<name, name> votes = _votes.get( proposal_name.value, "[proposal_name] does not exists" ).votes;
    const bool is_voting = votes.find( account ) != votes.end();

    // define account type (defined in order of proposer, voter, other)
    name account_type = "other"_n;
    if ( account == proposal.proposer ) account_type = "proposer"_n;
    else if ( is_voting ) account_type = "voter"_n;

    if ( comments_itr == _comments.end() ) {
        check( comment_json.size(), "[comment_json] is empty");

        _comments.emplace( ram_payer, [&]( auto& row ) {
            row.account = account;
            row.account_type = account_type;
            row.timestamp = current_time_point();
            row.version = 0;
            row.comment_json = comment_json;
        });
    } else {
        check( proposal.status == "active"_n, "cannot modify/erase comment when proposal is no longer active");

        // modify comment
        if ( comment_json.size() ) {
            _comments.modify( comments_itr, same_payer, [&]( auto& row ) {
                row.account_type = account_type;
                row.timestamp = current_time_point();
                row.version += 1;
                row.comment_json = comment_json;
            });
        // delete comment
        } else {
            _comments.erase( comments_itr );
        }
    }
}
