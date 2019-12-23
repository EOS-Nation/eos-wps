/**
 * TESTING ONLY
 *
 * Should be removed in production
 */
void wps::add_transfer( const eosio::name type, const eosio::name from, const eosio::name to, const eosio::asset quantity, const eosio::string memo )
{
    _transfers.emplace( get_self(), [&]( auto& row ) {
        row.id          = _transfers.available_primary_key();
        row.type        = type;
        row.from        = from;
        row.to          = to;
        row.quantity    = quantity;
        row.memo        = memo;
        row.timestamp   = current_time_point();
        row.tx_id       = get_tx_id();
    });
}
