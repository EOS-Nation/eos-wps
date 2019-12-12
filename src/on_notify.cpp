namespace eosio {

[[eosio::on_notify("eosio.token::transfer")]]
void wps::transfer( const eosio::name&    from,
                    const eosio::name&    to,
                    const eosio::asset&   quantity,
                    const eosio::string&  memo )
{
    require_auth( from );

    // Only monitor incoming transfers to get_self() account
    if ( to != get_self() ) return;

    // exclude system account
    if ( from == "eosio.ram"_n || from == "eosio.stake"_n ) return;

    // funding WPS using memo="funding" or from designated system account
    if ( memo == "funding" || from == "eosio.saving"_n || from == "eosio.names"_n || from == "eosio.ramfee"_n ) {
        add_funding( quantity );
        return;
    }

    // // funding WPS proposal deposit requirements
    // // validate memo
    // check( memo.length() > 0, "memo is required");
    // check( memo.length() <= 12, "memo must be 12 characters or less");
    // const eosio::name proposal_name = name{ memo };

    // deposit quantity to account
    auto deposits_itr = _deposits.find( from.value );
    check( deposits_itr != _deposits.end(), "deposit account does not exist, must `submitdraft` action before sending funds to " + get_self().to_string());

    add_deposit( from, quantity );
    add_liquid_deposits( quantity );
}

}