// @action
void wps::refund( const eosio::name account )
{
    require_auth( account );

    auto deposits_itr = _deposits.find( account.value );

    // validation
    check( deposits_itr != _deposits.end(), "[account] does not exists");
    check( deposits_itr->balance.amount > 0, "account has no amount to refund");

    // send liquid token
    const eosio::asset remaining_balance = deposits_itr->balance;
    token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
    transfer.send( get_self(), account, remaining_balance, "refund" );

    // TESTIN PURPOSES
    add_transfer("refund"_n, get_self(), account, remaining_balance, "refund" );

    // substract deposits
    sub_liquid_deposits( remaining_balance );
    sub_deposit( account, remaining_balance );
}

void wps::create_deposit_account( const eosio::name account )
{
    auto deposits_itr = _deposits.find( account.value );

    if ( deposits_itr == _deposits.end()) {
        _deposits.emplace( account, [&]( auto& row ) {
            row.account = account;
            row.balance = asset{0, symbol{ "EOS", 4 }};
        });
    }
}

void wps::add_deposit( const eosio::name account, const eosio::asset quantity )
{
    auto deposits_itr = _deposits.find( account.value );

    // create deposit row
    if (deposits_itr == _deposits.end()) {
        _deposits.emplace( account, [&]( auto& row ) {
            row.account = account;
            row.balance = quantity;
        });
    } else {
        _deposits.modify( deposits_itr, same_payer, [&]( auto& row ) {
            row.balance += quantity;
        });
    }
}

void wps::sub_deposit( const eosio::name account, const eosio::asset quantity )
{
    auto deposits_itr = _deposits.find( account.value );

    check( deposits_itr != _deposits.end(), "[account] does not contain a balance" );

    _deposits.modify( deposits_itr, same_payer, [&]( auto& row ) {
        row.balance -= quantity;
        check( row.balance.amount >= 0, "post-balance cannot be less than 0");
    });
}

void wps::add_liquid_deposits( const eosio::asset quantity )
{
    auto state = _state.get_or_default();
    state.liquid_deposits += quantity;
    _state.set( state, get_self() );
}

void wps::sub_liquid_deposits( const eosio::asset quantity )
{
    auto state = _state.get_or_default();
    state.liquid_deposits -= quantity;
    _state.set( state, get_self() );
}

void wps::move_to_locked_deposits( const eosio::asset quantity )
{
    auto state = _state.get_or_default();
    state.liquid_deposits -= quantity;
    state.locked_deposits += quantity;
    _state.set( state, get_self() );
}