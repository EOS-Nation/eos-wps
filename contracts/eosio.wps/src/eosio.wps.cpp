#include "eosio.wps/eosio.wps.hpp"

namespace eosio {

void wps::propose(const eosio::name proposer,
                  const eosio::name proposal_name,
                  const string title,
                  const string proposal_json,
                  const eosio::asset budget,
                  const uint8_t payments )
{
    print("foo");
}

void wps::vote( const eosio::name voter, const eosio::name proposal_name, const eosio::name vote )
{
    print("foo");
}

}