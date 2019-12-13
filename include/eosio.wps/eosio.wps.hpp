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

static constexpr uint64_t DAY = 86400; // 24 hours
static constexpr uint64_t WEEK = 604800; // 7 days
static constexpr uint64_t MONTH = 2592000; // 30 days

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

typedef eosio::singleton< "settings"_n, wps_parameters> settings_table;

/**
 * ## TABLE `drafts`
 *
 * - scope: `proposer`
 *
 * - `{name} proposer` - proposer of proposal
 * - `{name} proposal_name` - proposal name
 * - `{string} title` - proposal title
 * - `{asset} monthly_budget` - monthly budget payment request
 * - `{uint8_t} duration` - monthly budget duration (maximum of 6 months)
 * - `{asset} total_budget` - total budget payment request
 * - `{map<name, string>} proposal_json` - a sorted container of <key, value>
 *
 * ### example
 *
 * ```json
 * {
 *   "proposer": "myaccount",
 *   "proposal_name": "mywps",
 *   "title": "My WPS",
 *   "monthly_budget": "500.0000 EOS",
 *   "duration": 2,
 *   "total_budget": "1000.0000 EOS",
 *   "proposal_json": [
 *     { "key": "category", "value": "other" },
 *     { "key": "region", "value": "global" }
 *   ]
 * }
 * ```
 */
struct [[eosio::table("drafts"), eosio::contract("eosio.wps")]] drafts_row {
    eosio::name                             proposer;
    eosio::name                             proposal_name;
    eosio::string                           title;
    eosio::asset                            monthly_budget;
    uint8_t                                 duration;
    eosio::asset                            total_budget;
    std::map<eosio::name, eosio::string>    proposal_json;

    uint64_t primary_key() const { return proposal_name.value; }
};
typedef eosio::multi_index< "drafts"_n, drafts_row> drafts_table;

/**
 * ## TABLE `proposers`
 *
 * - `{name} proposer` - proposer of proposal
 * - `{map<name, string>} metadata_json` - a sorted container of <key, value>
 *
 * ### example
 *
 * ```json
 * {
 *   "proposer": "myaccount",
 *   "metadata_json": [
 *     { "key": "region", "value": "global" }
 *   ]
 * }
 * ```
 */
struct [[eosio::table("proposers"), eosio::contract("eosio.wps")]] proposers_row {
    eosio::name                             proposer;
    std::map<eosio::name, eosio::string>    metadata_json;

    uint64_t primary_key() const { return proposer.value; }
};
typedef eosio::multi_index< "proposers"_n, proposers_row> proposers_table;

/**
 * ## TABLE `proposals`
 *
 * - `{name} proposer` - proposer of proposal
 * - `{name} proposal_name` - proposal name
 * - `{string} title` - proposal title
 * - `{asset} monthly_budget` - monthly budget payment request
 * - `{uint8_t} duration` - monthly budget duration (maximum of 6 months)
 * - `{asset} total_budget` - total budget payment request
 * - `{map<name, string>} proposal_json` - a sorted container of <key, value>
 * - `{time_point_sec} activated` - time proposal was activated (UTC)
 * - `{time_point_sec} start` - start of voting period (UTC)
 * - `{time_point_sec} end` - end of voting period (UTC)
 *
 * ### example
 *
 * ```json
 * {
 *   "proposer": "myaccount",
 *   "proposal_name": "mywps",
 *   "title": "My WPS",
 *   "monthly_budget": "500.0000 EOS",
 *   "duration": 2,
 *   "total_budget": "1000.0000 EOS",
 *   "proposal_json": [
 *     { "key": "category", "value": "other" },
 *     { "key": "region", "value": "global" }
 *   ],
 *   "created": "2019-11-05T12:10:00",
 *   "start": "2019-11-01T00:00:00",
 *   "end": "2019-12-01T00:00:00"
 * }
 * ```
 */
struct [[eosio::table("proposals"), eosio::contract("eosio.wps")]] proposals_row : drafts_row {
    eosio::name                             proposer;
    eosio::name                             proposal_name;
    eosio::string                           title;
    eosio::asset                            monthly_budget;
    uint8_t                                 duration;
    eosio::asset                            total_budget;
    std::map<eosio::name, eosio::string>    proposal_json;
    eosio::time_point_sec                   created;
    eosio::time_point_sec                   start;
    eosio::time_point_sec                   end;

    uint64_t primary_key() const { return proposal_name.value; }
};

typedef eosio::multi_index< "proposals"_n, proposals_row> proposals_table;

/**
 * ## TABLE `votes`
 *
 * - `{name} proposal_name` - The proposal's name, its ID among all proposals
 * - `{int16_t} total_net_votes` - total net votes
 * - `{map<name, name>} votes` - a sorted container of <voter, vote>
 *
 * ### example
 *
 * ```json
 * {
 *   "proposal_name": "mywps",
 *   "total_net_votes": 2,
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
    int16_t                             total_net_votes;
    std::map<eosio::name, eosio::name>  votes;

    uint64_t primary_key() const { return proposal_name.value; }
};

typedef eosio::multi_index< "votes"_n, votes_row> votes_table;

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

typedef eosio::singleton< "state"_n, state_row> state_table;

/**
 * ## TABLE `deposits`
 *
 * - `{name} account` - account balance owner
 * - `{asset} balance` - token balance amount
 *
 * ### example
 *
 * ```json
 * {
 *   "account": "myaccount",
 *   "balance": "100.0000 EOS"
 * }
 * ```
 */
struct [[eosio::table("deposits"), eosio::contract("eosio.wps")]] deposits_row {
    eosio::name         account;
    eosio::asset        balance;

    uint64_t primary_key() const { return account.value; }
};

typedef eosio::multi_index< "deposits"_n, deposits_row > deposits_table;

/**
 * ## TABLE `funding`
 *
 * - `{uint64_t} id` - incoming transfer identifier
 * - `{name} from` - sender of transfer
 * - `{asset} quantity` - transfer quantity amount
 * - `{string} memo` - transfer memo
 * - `{checksum256} tx_id` - transaction ID
 *
 * ### example
 *
 * ```json
 * {
 *   "id": 0,
 *   "from": "myaccount",
 *   "quantity": "50.0000 EOS",
 *   "memo": "donation",
 *   "tx_id": "<TRANSACTION ID>"
 * }
 * ```
 */
struct [[eosio::table("funding"), eosio::contract("eosio.wps")]] funding_row {
    uint64_t            id;
    eosio::name         from;
    eosio::asset        quantity;
    eosio::string       memo;
    eosio::checksum256  tx_id;

    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index< "funding"_n, funding_row > funding_table;

/**
 * ## TABLE `periods`
 *
 * - `{time_point_sec} period` - current voting period
 * - `{set<name>} proposals` - set of proposal names
 *
 * ### example
 *
 * ```json
 * {
 *   "period": "2019-11-01T00:00:00",
 *   "proposals": ["mywps"],
 * }
 * ```
 */
struct [[eosio::table("periods"), eosio::contract("eosio.wps")]] periods_row {
    eosio::time_point_sec       period;
    std::set<eosio::name>       proposals;

    uint64_t primary_key() const { return period.sec_since_epoch(); }
};

typedef eosio::multi_index< "periods"_n, periods_row > periods_table;

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
            _proposals( get_self(), get_self().value ),
            _votes( get_self(), get_self().value ),
            _settings( get_self(), get_self().value ),
            _state( get_self(), get_self().value ),
            _deposits( get_self(), get_self().value ),
            _proposers( get_self(), get_self().value ),
            _periods( get_self(), get_self().value ),
            _funding( get_self(), get_self().value )
    {}

    /**
     * ## ACTION `submitdraft`
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
     * - `{asset} monthly_budget` - monthly budget payment request
     * - `{uin8_t} duration` - monthly budget duration (maximum of 6 months)
     * - `{map<name, string>} proposal_json` - a sorted container of <key, value>
     *
     * ### example
     *
     * ```bash
     * cleos push action eosio.wps submitdraft '["myaccount", "mywps", "My WPS", "500.0000 EOS", 1, [{"key":"category", "value":"other"}, {"key":"region", "value":"global"}]]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void submitdraft(const eosio::name proposer,
                     const eosio::name proposal_name,
                     const eosio::string title,
                     const eosio::asset monthly_budget,
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
     * Refund any remaining deposit amount from requesting account
     *
     * - Authority:  `account`
     *
     * - `{name} account` - account requesting refund
     *
     * ```bash
     * cleos push action eosio.wps refund '["myaccount"]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void refund( const eosio::name account );

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
     * ## ACTION `setproposer`
     *
     * Set proposer's metadata
     *
     * - Authority:  `proposer`
     *
     * ### params
     *
     * - `{name} proposer` - proposer of proposal
     * - `{map<name, string>} metadata_json` - a sorted container of <key, value>
     *
     * ### example
     *
     * ```bash
     * cleos push action eosio.wps setproposer '["myaccount", [{"key":"region", value":"global"}]]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void setproposer(const eosio::name proposer, const std::map<name, string> metadata_json );

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
    using submitdraft_action = eosio::action_wrapper<"submitdraft"_n, &wps::submitdraft>;
    using activate_action = eosio::action_wrapper<"activate"_n, &wps::activate>;
    using refund_action = eosio::action_wrapper<"refund"_n, &wps::refund>;
    using canceldraft_action = eosio::action_wrapper<"canceldraft"_n, &wps::canceldraft>;
    using modifydraft_action = eosio::action_wrapper<"modifydraft"_n, &wps::modifydraft>;
    using init_action = eosio::action_wrapper<"init"_n, &wps::init>;
    using setparams_action = eosio::action_wrapper<"setparams"_n, &wps::setparams>;

private:
    // local instances of the multi indexes
    proposals_table             _proposals;
    votes_table                 _votes;
    settings_table              _settings;
    state_table                 _state;
    deposits_table              _deposits;
    proposers_table             _proposers;
    periods_table               _periods;
    funding_table               _funding;

    // private helpers
    int16_t calculate_total_net_votes( const std::map<eosio::name, eosio::name> votes );
    void setperiod();
    void move_to_locked_deposits( const eosio::asset quantity );
    void deposit_to_proposal( const eosio::name proposal_name, const eosio::asset quantity );
    void check_title( const string title );
    void proposal_to_periods( const eosio::name proposal_name, const uint8_t duration, const eosio::name ram_payer );
    eosio::checksum256 get_tx_id();

    // funding
    void add_funding_transfer( const eosio::name from, const eosio::asset quantity, const eosio::string memo );
    void add_funding( const eosio::asset quantity );
    void sub_funding( const eosio::asset quantity );

    // deposits
    void add_deposit( const eosio::name account, const eosio::asset quantity );
    void sub_deposit( const eosio::name account, const eosio::asset quantity );
    void create_deposit_account( const eosio::name account );
    void add_liquid_deposits( const eosio::asset quantity );
    void sub_liquid_deposits( const eosio::asset quantity );
};

}