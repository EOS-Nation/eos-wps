namespace eosio {

[[eosio::on_notify("eosio.token::transfer")]]
void wps::transfer( const name&    from,
                    const name&    to,
                    const asset&   quantity,
                    const string&  memo )
{
    require_auth( from );

    // Only monitor incoming transfers to get_self() account
    if ( to != get_self() ) return;

    // exclude system account
    if ( from == "eosio.ram"_n || from == "eosio.stake"_n ) return;

    // validate memo
    check( memo.length() > 0, "memo is required");
    check( memo.length() <= 12, "memo must be 12 characters or less");

    // validate proposal
    auto proposals_itr = _proposals.find( name{ memo }.value );
    auto settings = _settings.get_or_default();

    check( proposals_itr != _proposals.end(), "memo does not match any proposal");
    check( proposals_itr->proposer == from, "`from` must match `proposer` of propsal");
    check( proposals_itr->status == "draft"_n, "proposal `status` must be in `draft`");
    check( quantity.symbol.code() == symbol_code("EOS"), "only EOS symbol code is allowed");

    // add deposit
    _proposals.modify( proposals_itr, from, [&]( auto& row ) {
        row.deposit += quantity;
        // prevent sending too much EOS for activation deposit
        check( row.deposit <= settings.deposit_required, "activation deposit cannot be greater than " + settings.deposit_required.to_string());
    });
}

}