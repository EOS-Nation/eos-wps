#pragma once

#include <eosio/producer_schedule.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include <optional>
#include <string>

using namespace eosio;
using namespace std;

/**
* Defines `producer_info` structure to be stored in `producer_info` table, added after version 1.0
*/
struct [[eosio::table("producers"), eosio::contract("eosio.system")]] producer_info {
    name                  owner;
    double                total_votes = 0;
    eosio::public_key     producer_key; /// a packed public key object
    bool                  is_active = true;
    std::string           url;
    uint32_t              unpaid_blocks = 0;
    time_point            last_claim_time;
    uint16_t              location = 0;

    uint64_t primary_key()const { return owner.value;                             }
    double   by_votes()const    { return is_active ? -total_votes : total_votes;  }
    bool     active()const      { return is_active;                               }
    void     deactivate()       { producer_key = public_key(); is_active = false; }

    // explicit serialization macro is not necessary, used here only to improve compilation time
    EOSLIB_SERIALIZE( producer_info, (owner)(total_votes)(producer_key)(is_active)(url)
                    (unpaid_blocks)(last_claim_time)(location) )
};

/**
* Defines producer info table added in version 1.0
*/
typedef eosio::multi_index< "producers"_n, producer_info,
    indexed_by<"prototalvote"_n, const_mem_fun<producer_info, double, &producer_info::by_votes>  >
> producers_table;
