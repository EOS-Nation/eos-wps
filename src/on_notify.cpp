[[eosio::on_notify("eosio.token::transfer")]]
void wps::transfer( const eosio::name&    from,
                    const eosio::name&    to,
                    const eosio::asset&   quantity,
                    const eosio::string&  memo )
{
    require_auth( from );
    const eosio::name ram_payer = from;

    check( _state.exists(), "contract not yet initialized" );
    check( _settings.exists(), "settings are missing" );

    // Only monitor incoming transfers to get_self() account
    if ( to != get_self() ) return;

    // exclude system account
    if ( from == "eosio.ram"_n || from == "eosio.stake"_n ) return;

    // funding WPS from designated system account
    // funding WPS using donations (memo="donate")
    if ( memo == "donate" || from == "eosio.names"_n || from == "eosio.ramfee"_n ) {
        add_funding( quantity );
        return;
    }

    // deposit quantity to account
    eosio::name deposit_to = from;

    // accept deposits from alternate account
    if ( memo.size() ) {
        check( is_account( name{ memo } ), "memo must be an active account name");
        deposit_to = name{ memo };
    }

    auto deposits_itr = _deposits.find( deposit_to.value );
    check( deposits_itr != _deposits.end(), "deposit account does not exist, must `submitdraft` action before sending funds to " + get_self().to_string());

    add_deposit( deposit_to, quantity, ram_payer );
    add_liquid_deposits( quantity );
}