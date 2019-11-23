#pragma once

#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>

#include <eosio.token/eosio.token.hpp>

using namespace eosio;
using namespace std;

namespace eosio {

class [[eosio::contract("eosio.wps")]] wps : public contract {
public:
    using contract::contract;

    /**
     * Construct a new contract given the contract name
     *
     * @param {name} receiver - The name of this contract
     * @param {name} code - The code name of the action this contract is processing.
     * @param {datastream} ds - The datastream used
     */
    wps( name receiver, name code, eosio::datastream<const char*> ds )
        : contract( receiver, code, ds ),
            _settings( get_self(), get_self().value ),
            _proposals( get_self(), get_self().value ),
            _votes( get_self(), get_self().value )
    {}

    /**
     * ## ACTION `propose`
     *
     * Submit a WPS proposal
     *
     * - Authority:  `proposer`
     *
     * ### params
     *
     * - `{name} proposer` - proposer of proposal
     * - `{name} proposal_name` - proposal name
     * - `{string} title` - proposal title
     * - `{asset} budget` - monthly budget payment request
     * - `{uin8_t} payments` - number of monthly payment duration (maximum of 6 months)
     * - `{map<name, string>} proposal_json` - a sorted container of <key, value>
     *
     * ### example
     *
     * ```bash
     * cleos push action eosio.wps propose '["myaccount", "mywps", "My WPS", "500.0000 EOS", 1, [["category", "other"], ["region", "global"]]]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void propose(const eosio::name proposer,
                 const eosio::name proposal_name,
                 const eosio::string title,
                 const eosio::asset budget,
                 const uint8_t payments,
                 const std::map<eosio::name, eosio::string> proposal_json );

    /**
     * ## ACTION `vote`
     *
     * Vote for a WPS proposal
     *
     * - Authority:  `voter`
     *
     * - `{name} voter` - voter
     * - `{name} proposal_name` - proposal name
     * - `{name} vote` - vote (yes/no/abstain)
     *
     * ```bash
     * cleos push action eosio.wps vote '["myaccount", "mywps", "yes"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void vote( const eosio::name voter, const eosio::name proposal_name, const eosio::name vote );

    [[eosio::action]]
    void init( const eosio::time_point_sec current_voting_period );

    [[eosio::action]]
    void activate( const eosio::name proposer, const eosio::name proposal_name );

    [[eosio::action]]
    void settings( const uint64_t vote_margin, const eosio::asset deposit_required, const uint64_t voting_interval );

    [[eosio::action]]
    void refund( const eosio::name proposer, const eosio::name proposal_name );

    [[eosio::action]]
    void canceldraft( const eosio::name proposer, const eosio::name proposal_name );

    [[eosio::on_notify("eosio.token::transfer")]]
    void transfer( const eosio::name&    from,
                   const eosio::name&    to,
                   const eosio::asset&   quantity,
                   const eosio::string&  memo );

    using vote_action = eosio::action_wrapper<"vote"_n, &wps::vote>;
    using propose_action = eosio::action_wrapper<"propose"_n, &wps::propose>;
    using init_action = eosio::action_wrapper<"init"_n, &wps::init>;
    using activate_action = eosio::action_wrapper<"activate"_n, &wps::activate>;
    using settings_action = eosio::action_wrapper<"settings"_n, &wps::settings>;
    using refund_action = eosio::action_wrapper<"refund"_n, &wps::refund>;
    using canceldraft_action = eosio::action_wrapper<"canceldraft"_n, &wps::canceldraft>;

private:
    /**
     * ## TABLE `proposals`
     *
     * - `{name} proposer` - proposer of proposal
     * - `{name} proposal_name` - proposal name
     * - `{string} title` - proposal title
     * - `{asset} budget` - monthly budget payment request
     * - `{uint8_t} payments` - number of monthly payment duration (maximum of 6 months)
     * - `{asset} deposit` - deposit required to active proposal
     * - `{name} status` - current status of proposal (draft/active/completed/expired)
     * - `{map<name, string>} proposal_json` - a sorted container of <key, value>
     *
     * ### example
     *
     * ```json
     * {
     *   "proposer": "myaccount",
     *   "proposal_name": "mywps",
     *   "title": "My WPS",
     *   "budget": "500.0000 EOS",
     *   "payments": 1,
     *   "deposit": "0.0000 EOS",
     *   "status": "draft",
     *   "proposal_json": [
     *     { "key": "category", "value": "other" },
     *     { "key": "region", "value": "global" }
     *   ]
     * }
     * ```
     */
    struct [[eosio::table("proposals")]] proposals_row {
        eosio::name                             proposer;
        eosio::name                             proposal_name;
        eosio::string                           title;
        eosio::asset                            budget;
        uint8_t                                 payments;
        eosio::asset                            deposit;
        eosio::name                             status;
        std::map<eosio::name, eosio::string>    proposal_json;

        uint64_t primary_key() const { return proposal_name.value; }
    };

    /**
     * ## TABLE `votes`
     *
     * - `{name} proposal_name` - The proposal's name, its ID among all proposals
     * - `{int16_t} total_net_votes` - total net votes
     * - `{time_point_sec} start` - start of voting period (UTC)
     * - `{time_point_sec} end` - end of voting period (UTC)
     * - `{map<name, name>} votes` - a sorted container of <voter, vote>
     *
     * ### example
     *
     * ```json
     * {
     *   "proposal_name": "mywps",
     *   "total_net_votes": 2,
     *   "start": "2019-11-01T00:00:00",
     *   "end": "2019-12-01T00:00:00",
     *   "votes": [
     *      { "key": "mybp1", "value": "yes" },
     *      { "key": "mybp2", "value": "no" },
     *      { "key": "mybp3", "value": "yes" },
     *      { "key": "mybp4", "value": "abstain" },
     *      { "key": "mybp5", "value": "yes" }
     *   ]
     * }
     * ```
     */
    struct [[eosio::table("votes")]] votes_row {
        eosio::name                 proposal_name;
        int16_t                     total_net_votes;
        eosio::time_point_sec       start;
        eosio::time_point_sec       end;
        std::map<eosio::name, eosio::name>  votes;

        uint64_t primary_key() const { return proposal_name.value; }
    };

    /**
     * ## TABLE `settings`
     *
     * - `{time_point_sec} current_voting_period` - current voting period
     * - `{int16_t} [vote_margin=15]` - minimum BP vote margin threshold to reach for proposals
     * - `{asset} [deposit_required="100.0000 EOS"]` - deposit required to active proposal
     * - `{uint64_t} [voting_interval=2592000]` -  election interval in seconds
     *
     * ### example
     *
     * ```json
     * {
     *   "current_voting_period": "2019-11-01T00:00:00",
     *   "vote_margin": 15,
     *   "deposit_required": "100.0000 EOS",
     *   "voting_interval": 2592000,
     * }
     * ```
     */
    struct [[eosio::table("settings")]] settings_row {
        eosio::time_point_sec       current_voting_period;
        int16_t                     vote_margin = 15;
        eosio::asset                deposit_required = asset{1000000, symbol{"EOS", 4}};
        uint64_t                    voting_interval = 2592000;
    };

    // Tables
    typedef eosio::multi_index< "proposals"_n, proposals_row> proposals_table;
    typedef eosio::multi_index< "votes"_n, votes_row> votes_table;
    typedef eosio::singleton< "settings"_n, settings_row> settings_table;

    // local instances of the multi indexes
    proposals_table             _proposals;
    settings_table              _settings;
    votes_table                 _votes;

    // private helpers
    int16_t calculate_total_net_votes( const std::map<eosio::name, eosio::name> votes );
};

}