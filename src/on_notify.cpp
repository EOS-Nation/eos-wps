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

    // funding WPS using memo="donate" or from designated system account
    if ( memo == "donate" || from == "eosio.saving"_n || from == "eosio.names"_n || from == "eosio.ramfee"_n ) {
        add_funding( quantity );
        add_funding_transfer( from, quantity, memo );
        return;
    }

    // deposit quantity to account
    auto deposits_itr = _deposits.find( from.value );
    check( deposits_itr != _deposits.end(), "deposit account does not exist, must `submitdraft` action before sending funds to " + get_self().to_string());

    add_deposit( from, quantity );
    add_liquid_deposits( quantity );
}