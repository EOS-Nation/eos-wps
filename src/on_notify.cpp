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
    const eosio::name proposal_name = name{ memo };

    // deposit quantity to proposal
    deposit_to_proposal( proposal_name, quantity );
    add_liquid_deposits( quantity );
}

}