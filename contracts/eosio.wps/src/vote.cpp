namespace eosio {

void wps::vote( const eosio::name voter, const eosio::name proposal_name, const eosio::name vote )
{
    require_auth( voter );

    auto proposals_itr = _proposals.find( proposal_name.value );
    check( proposals_itr != _proposals.end(), "[proposal_name] does not exists");
    check( proposals_itr->status == "active"_n, "[proposal_name] is not active");

    votes_table _votes( get_self(), proposal_name.value );
    auto votes_itr = _votes.find( voter.value );

    // vote does not exist from voter
    if (votes_itr == _votes.end()) {
        _votes.emplace( voter, [&]( auto& row ) {
            row.voter = vote;
        });
    } else {
        _votes.modify( votes_itr, same_payer, [&]( auto& row ) {
            row.voter = vote;
        });
    }
}

}