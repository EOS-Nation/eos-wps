#pragma once

#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>

#include <optional>

#include <eosio.token/eosio.token.hpp>

using namespace eosio;
using namespace std;

static constexpr bool TESTING = true;

namespace eosio {

/**
 * ## TABLE `settings`
 *
 * - `{int16_t} [vote_margin=15]` - minimum BP vote margin threshold to reach for proposals
 * - `{asset} [deposit_required="100.0000 EOS"]` - deposit required to active proposal
 * - `{uint64_t} [voting_interval=2592000]` -  election interval in seconds
 * - `{asset} [max_monthly_budget="50000.0000 EOS"]` - maximum monthly budget
 *
 * ### example
 *
 * ```json
 * {
 *   "vote_margin": 15,
 *   "deposit_required": "100.0000 EOS",
 *   "voting_interval": 2592000,
 *   "max_monthly_budget": "50000.0000 EOS"
 * }
 * ```
 */
struct [[eosio::table("settings"), eosio::contract("eosio.wps")]] wps_parameters {
    int16_t             vote_margin = 15;
    eosio::asset        deposit_required = asset{ 1000000, symbol{"EOS", 4}};
    uint64_t            voting_interval = 2592000;
    eosio::asset        max_monthly_budget = asset{ 500000000, symbol{"EOS", 4}};
};

/**
 * ## TABLE `proposals`
 *
 * - `{name} proposer` - proposer of proposal
 * - `{name} proposal_name` - proposal name
 * - `{string} title` - proposal title
 * - `{asset} budget` - monthly budget payment request
 * - `{uint8_t} duration` - monthly budget duration (maximum of 6 months)
 * - `{name} status` - current status of proposal (draft/active/completed/expired)
 * - `{asset} deposit` - deposit required to active proposal
 * - `{asset} payments` - payments made to WPS proposal
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
 *   "duration": 1,
 *   "status": "draft",
 *   "deposit": "0.0000 EOS",
 *   "payments": "0.0000 EOS",
 *   "proposal_json": [
 *     { "key": "category", "value": "other" },
 *     { "key": "region", "value": "global" }
 *   ]
 * }
 * ```
 */
struct [[eosio::table("proposals"), eosio::contract("eosio.wps")]] proposals_row {
    eosio::name                             proposer;
    eosio::name                             proposal_name;
    eosio::string                           title;
    eosio::asset                            budget;
    uint8_t                                 duration;
    eosio::name                             status;
    eosio::asset                            deposit;
    eosio::asset                            payments;
    std::map<eosio::name, eosio::string>    proposal_json;

    uint64_t primary_key() const { return proposal_name.value; }
    uint64_t by_status() const { return status.value; }
};

/**
 * ## TABLE `votes`
 *
 * - `{name} proposal_name` - The proposal's name, its ID among all proposals
 * - `{name} status` - current status of proposal (draft/active/completed/expired)
 * - `{int16_t} total_net_votes` - total net votes
 * - `{time_point_sec} voting_period` - active voting period (UTC)
 * - `{time_point_sec} start` - start of voting period (UTC)
 * - `{time_point_sec} end` - end of voting period (UTC)
 * - `{map<name, name>} votes` - a sorted container of <voter, vote>
 *
 * ### example
 *
 * ```json
 * {
 *   "proposal_name": "mywps",
 *   "status": "active",
 *   "total_net_votes": 2,
 *   "voting_period": "2019-11-01T00:00:00",
 *   "start": "2019-11-05T12:10:00",
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
struct [[eosio::table("votes"), eosio::contract("eosio.wps")]] votes_row {
    eosio::name                         proposal_name;
    eosio::name                         status;
    int16_t                             total_net_votes;
    eosio::time_point_sec               voting_period;
    eosio::time_point_sec               start;
    eosio::time_point_sec               end;
    std::map<eosio::name, eosio::name>  votes;

    uint64_t primary_key() const { return proposal_name.value; }
    uint64_t by_status() const { return status.value; }
    uint64_t by_voting_period() const { return voting_period.sec_since_epoch(); }
};

/**
 * ## TABLE `state`
 *
 * - `{time_point_sec} current_voting_period` - current voting period
 * - `{asset} [liquid_deposits="0.0000 EOS"]` - liquid deposits
 * - `{asset} [locked_deposits="0.0000 EOS"]` - locked deposits
 * - `{asset} [available_funding="0.0000 EOS"]` - available funding
 *
 * ### example
 *
 * ```json
 * {
 *   "current_voting_period": "2019-11-01T00:00:00",
 *   "liquid_deposits": "100.0000 EOS",
 *   "locked_deposits": "200.0000 EOS",
 *   "available_funding": "50000.0000 EOS",
 * }
 * ```
 */
struct [[eosio::table("state"), eosio::contract("eosio.wps")]] state_row {
    eosio::time_point_sec       current_voting_period;
    eosio::asset                liquid_deposits;
    eosio::asset                locked_deposits;
    eosio::asset                available_funding;
};

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
            _proposals( get_self(), get_self().value ),
            _votes( get_self(), get_self().value ),
            _settings( get_self(), get_self().value ),
            _state( get_self(), get_self().value )
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
     * - `{uin8_t} duration` - monthly budget duration (maximum of 6 months)
     * - `{map<name, string>} proposal_json` - a sorted container of <key, value>
     *
     * ### example
     *
     * ```bash
     * cleos push action eosio.wps propose '["myaccount", "mywps", "My WPS", "500.0000 EOS", 1, [{"key":"category", "value":"other"}, {"key":"region", "value":"global"}]]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void propose(const eosio::name proposer,
                 const eosio::name proposal_name,
                 const eosio::string title,
                 const eosio::asset budget,
                 const uint8_t duration,
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

    /**
     * ## ACTION `activate`
     *
     * Activate WPS proposal
     *
     * - Authority:  `proposer`
     *
     * - `{name} proposer` - proposer
     * - `{name} proposal_name` - proposal name
     *
     * ```bash
     * cleos push action eosio.wps activate '["myaccount", "mywps"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void activate( const eosio::name proposer, const eosio::name proposal_name );

    /**
     * ## ACTION `refund`
     *
     * Refund any remaining deposit amount from a draft WPS proposal.
     *
     * - Authority:  `proposer`
     *
     * - `{name} proposer` - proposer
     * - `{name} proposal_name` - proposal name
     *
     * ```bash
     * cleos push action eosio.wps refund '["myaccount", "mywps"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void refund( const eosio::name proposer, const eosio::name proposal_name );

    /**
     * ## ACTION `canceldraft`
     *
     * Cancel draft WPS proposal
     *
     * - Authority:  `proposer`
     *
     * - `{name} proposer` - proposer
     * - `{name} proposal_name` - proposal name
     *
     * ```bash
     * cleos push action eosio.wps canceldraft '["myaccount", "mywps"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void canceldraft( const eosio::name proposer, const eosio::name proposal_name );

    /**
     * ## ACTION `modifydraft`
     *
     * Modify draft WPS proposal
     *
     * - Authority:  `proposer`
     *
     * ### params
     *
     * - `{name} proposer` - proposer of proposal
     * - `{name} proposal_name` - proposal name
     * - `{string} title` - proposal title
     * - `{map<name, string>} proposal_json` - a sorted container of <key, value>
     *
     * ### example
     *
     * ```bash
     * cleos push action eosio.wps modifydraft '["myaccount", "mywps", "My WPS", [{"key":"region", "value":"global"}]]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void modifydraft(const eosio::name proposer, const eosio::name proposal_name, const string title, const std::map<name, string> proposal_json );

    /**
     * ## ACTION `init`
     *
     * Initialize WPS contract
     *
     * - Authority:  `get_self()`
     *
     * - `{time_point_sec} initial_voting_period` - initial voting period
     *
     * ```bash
     * cleos push action eosio.wps init '["2019-11-25T00:00:00"]' -p eosio.wps
     * ```
     */
    [[eosio::action]]
    void init( const eosio::time_point_sec current_voting_period );

    /**
     * ## ACTION `setparams`
     *
     * Set paramaters for WPS contract
     *
     * - Authority:  `get_self()`
     *
     * - `{int16_t} [vote_margin=15]` - minimum BP vote margin threshold to reach for proposals
     * - `{asset} [deposit_required="100.0000 EOS"]` - deposit required to active proposal
     * - `{uint64_t} [voting_interval=2592000]` -  election interval in seconds
     *
     * ```bash
     * cleos push action eosio.wps setparams '[15, "100.0000 EOS", 2592000]' -p eosio.wps
     * ```
     */
    [[eosio::action]]
    void setparams( const wps_parameters params );

    /**
     * TESTING ONLY
     *
     * Should be removed in production
     */
    [[eosio::action]]
    void clean( const eosio::name table, const std::optional<eosio::name> scope );

    [[eosio::on_notify("eosio.token::transfer")]]
    void transfer( const eosio::name&    from,
                   const eosio::name&    to,
                   const eosio::asset&   quantity,
                   const eosio::string&  memo );

    using vote_action = eosio::action_wrapper<"vote"_n, &wps::vote>;
    using propose_action = eosio::action_wrapper<"propose"_n, &wps::propose>;
    using activate_action = eosio::action_wrapper<"activate"_n, &wps::activate>;
    using refund_action = eosio::action_wrapper<"refund"_n, &wps::refund>;
    using canceldraft_action = eosio::action_wrapper<"canceldraft"_n, &wps::canceldraft>;
    using init_action = eosio::action_wrapper<"init"_n, &wps::init>;
    using setparams_action = eosio::action_wrapper<"setparams"_n, &wps::setparams>;

private:
    // Tables
    typedef eosio::multi_index< "proposals"_n, proposals_row,
        indexed_by<"bystatus"_n, const_mem_fun<proposals_row, uint64_t, &proposals_row::by_status>>
    > proposals_table;

    typedef eosio::multi_index< "votes"_n, votes_row,
        indexed_by<"bystatus"_n, const_mem_fun<votes_row, uint64_t, &votes_row::by_status>>,
        indexed_by<"byperiod"_n, const_mem_fun<votes_row, uint64_t, &votes_row::by_voting_period>>
    > votes_table;

    typedef eosio::singleton< "settings"_n, wps_parameters> settings_table;
    typedef eosio::singleton< "state"_n, state_row> state_table;

    // local instances of the multi indexes
    proposals_table             _proposals;
    votes_table                 _votes;
    settings_table              _settings;
    state_table                 _state;

    // private helpers
    int16_t calculate_total_net_votes( const std::map<eosio::name, eosio::name> votes );
    void setperiod();
};

}