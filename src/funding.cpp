void wps::add_funding_transfer( const eosio::name from, const eosio::asset quantity, const eosio::string memo )
{
    _funding.emplace( get_self(), [&]( auto& row ) {
        row.id          = _funding.available_primary_key();
        row.from        = from;
        row.quantity    = quantity;
        row.memo        = memo;
        row.tx_id       = get_tx_id();
    });
}

void wps::add_funding( const eosio::asset quantity )
{
    auto state = _state.get_or_default();
    state.available_funding += quantity;
    _state.set( state, get_self() );
}

void wps::sub_funding( const eosio::asset quantity )
{
    auto state = _state.get_or_default();
    state.available_funding -= quantity;
    _state.set( state, get_self() );
}