[[eosio::action]]
void wps::comment( const name account, const name proposal_name, const map<name, string> comment_json )
{
    require_auth( account );
    const eosio::name ram_payer = account;

    comments_table _comments( get_self(), proposal_name.value );
    auto comments_itr = _comments.find( account.value );

    // only proposer and/or eligile voters are allowed to comment
    const auto proposal = _proposals.get( proposal_name.value, "[proposal_name] does not exists" );
    check( is_voter_eligible( account ) || proposal.proposer == account, "[account] not eligible to comment on proposal, must be proposer or eligible voter");

    if ( comments_itr == _comments.end() ) {
        check( comment_json.size(), "[comment_json] is empty");

        _comments.emplace( ram_payer, [&]( auto& row ) {
            row.account = account;
            row.timestamp = current_time_point();
            row.version = 0;
            row.comment_json = comment_json;
        });
    } else {
        check( proposal.status == "active"_n, "cannot modify/erase comment when proposal is no longer active");

        // modify comment
        if ( comment_json.size() ) {
            _comments.modify( comments_itr, same_payer, [&]( auto& row ) {
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
