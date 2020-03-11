#pragma once

#include <eosio/eosio.hpp>
#include <eosio/producer_schedule.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include <optional>
#include <string>

using namespace eosio;
using namespace std;

namespace eosiosystem {
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
    };

    /**
    * Defines producer info table added in version 1.0
    */
    typedef eosio::multi_index< "producers"_n, producer_info,
        indexed_by<"prototalvote"_n, const_mem_fun<producer_info, double, &producer_info::by_votes>  >
    > producers_table;

    // Defines new global state parameters.
    struct [[eosio::table("global"), eosio::contract("eosio.system")]] eosio_global_state {
        uint64_t free_ram()const { return max_ram_size - total_ram_bytes_reserved; }

        // blockchain_paramters
        uint64_t              max_block_net_usage;
        uint32_t              target_block_net_usage_pct;
        uint32_t              max_transaction_net_usage;
        uint32_t              base_per_transaction_net_usage;
        uint32_t              net_usage_leeway;
        uint32_t              context_free_discount_net_usage_num;
        uint32_t              context_free_discount_net_usage_den;
        uint32_t              max_block_cpu_usage;
        uint32_t              target_block_cpu_usage_pct;
        uint32_t              max_transaction_cpu_usage;
        uint32_t              min_transaction_cpu_usage;
        uint32_t              max_transaction_lifetime;
        uint32_t              deferred_trx_expiration_window;
        uint32_t              max_transaction_delay;
        uint32_t              max_inline_action_size;
        uint16_t              max_inline_action_depth;
        uint16_t              max_authority_depth;

        // global state
        uint64_t             max_ram_size = 64ll*1024 * 1024 * 1024;
        uint64_t             total_ram_bytes_reserved = 0;
        int64_t              total_ram_stake = 0;

        block_timestamp      last_producer_schedule_update;
        time_point           last_pervote_bucket_fill;
        int64_t              pervote_bucket = 0;
        int64_t              perblock_bucket = 0;
        uint32_t             total_unpaid_blocks = 0; /// all blocks which have been produced but not paid
        int64_t              total_activated_stake = 0;
        time_point           thresh_activated_stake_time;
        uint16_t             last_producer_schedule_size = 0;
        double               total_producer_vote_weight = 0; /// the sum of all producer votes
        block_timestamp      last_name_close;
    };

    typedef eosio::singleton< "global"_n, eosio_global_state >  global_state_singleton;

    // Local testing purposes
    class [[eosio::contract("eosio.system")]] system : public contract {
      public:
        using contract::contract;

        [[eosio::action]]
        void setproducer( const name producer, const double total_votes );
    };
}