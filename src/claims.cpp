// @action
void wps::claim( const eosio::name proposal_name )
{
    // no authorization required (can be executed by any account)

    // static actions
    token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });

    auto proposals_itr = _proposals.find( proposal_name.value );
    const eosio::name proposer = proposals_itr->proposer;
    const eosio::asset quantity = proposals_itr->claimable;

    check( proposals_itr != _proposals.end(), "[proposal_name] does not exists" );
    check( quantity.amount > 0, "no claimable amount" );

    transfer.send( get_self(), proposer, quantity, "wps" );
    add_claim( proposer, proposal_name, quantity );

    _proposals.modify( proposals_itr, same_payer, [&]( auto& row ) {
        row.claimable.amount = 0;
    });
}

void wps::add_claim( const eosio::name proposer, const eosio::name proposal_name, const eosio::asset quantity )
{
    _claims.emplace( get_self(), [&]( auto& row ) {
        row.id              = _claims.available_primary_key();
        row.proposer        = proposer;
        row.proposal_name   = proposal_name;
        row.quantity        = quantity;
        row.timestamp       = current_time_point();
        row.tx_id           = get_tx_id();
    });
}
