#include <eosio/transaction.hpp>
#include <eosio/crypto.hpp>

eosio::checksum256 wps::get_tx_id()
{
    size_t size = eosio::transaction_size();
    char buf[size];
    size_t read = eosio::read_transaction( buf, size );
    eosio::check( size == read, "read_transaction failed");
    return eosio::sha256( buf, read );
}
