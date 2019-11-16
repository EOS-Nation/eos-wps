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
     * - `{string} proposal_json` - proposal JSON metadata
     * - `{asset} budget` - monthly budget payment request
     * - `{uin8_t} payments` - number of monthly payment duration (maximum of 6 months)
     *
     * ### example
     *
     * ```bash
     * cleos push action eosio.wps propose '["myaccount", "mywps", "My WPS", "{\"category\": \"other\", \"region\": \"global\"}", "500.0000 EOS", 1]' -p myaccount
     * ```
     */
    [[eosio::action]]
    void propose(const eosio::name proposer,
                 const eosio::name proposal_name,
                 const string title,
                 const string proposal_json,
                 const eosio::asset budget,
                 const uint8_t payments );

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

    using vote_action = eosio::action_wrapper<"vote"_n, &wps::vote>;
    using propose_action = eosio::action_wrapper<"propose"_n, &wps::propose>;

private:
    /**
     * ## TABLE `proposal`
     *
     * - `{name} proposer` - proposer of proposal
     * - `{name} proposal_name` - proposal name
     * - `{string} title` - proposal title
     * - `{string} proposal_json` - proposal JSON metadata
     * - `{asset} budget` - monthly budget payment request
     * - `{uint8_t} payments` - number of monthly payment duration (maximum of 6 months)
     * - `{time_point_sec} last_updated` - last updated (in UTC)
     * - `{name} status` - current status of proposal (draft/active/completed/expired)
     *
     * ### example
     *
     * ```json
     * {
     *   "proposer": "myaccount",
     *   "proposal_name": "mywps",
     *   "title": "My WPS",
     *   "proposal_json": "{\"category\": \"other\", \"region\": \"global\"}",
     *   "budget": "500.0000 EOS",
     *   "payments": 1,
     *   "last_updated": "2019-11-03T16:48:21",
     *   "status": "draft"
     * }
     * ```
     */
    struct [[eosio::table("proposal")]] proposal_row {
        eosio::name                 proposer;
        eosio::name                 proposal_name;
        string                      title;
        string                      proposal_json;
        eosio::asset                budget;
        uint8_t                     payments;
        eosio::time_point_sec       last_updated;
        eosio::name                 status;

        uint64_t primary_key() const { return proposal_name.value; }
    };

    /**
     * ## TABLE `votes`
     *
     * - `{name} proposal_name` - The proposal's name, its ID among all proposals
     * - `{vector<name>} yes` - vector array of yes votes
     * - `{vector<name>} no` - vector array of no votes
     * - `{vector<name>} abstain` - vector array of abstain votes
     * - `{int16_t} total_net_votes` - total net votes
     * - `{time_point_sec} start` - start of voting period
     * - `{time_point_sec} end` - end of voting period
     *
     * ### example
     *
     * ```json
     * {
     *   "proposal_name": "mywps",
     *   "yes": ["mybp1", "mybp3", "mybp4"],
     *   "no": ["mybp2"],
     *   "abstain": [],
     *   "total_net_votes": 2,
     *   "start": "2019-11-00T00:00:00",
     *   "end": "2019-12-00T00:00:00"
     * }
     * ```
     */
    struct [[eosio::table("votes")]] votes_row {
        eosio::name                     proposal_name;
        std::vector<eosio::name>        yes;
        std::vector<eosio::name>        no;
        std::vector<eosio::name>        abstain;
        int16_t                         total_net_votes;
        eosio::time_point_sec           start;
        eosio::time_point_sec           end;

        uint64_t primary_key() const { return proposal_name.value; }
    };

    /**
     * ## TABLE `settings`
     *
     * - `{uint64_t} vote_margin` - minimum BP vote margin threshold to reach for proposals
     * - `{time_point_sec} current` - current voting period
     *
     * ### example
     *
     * ```json
     * {
     *   "vote_margin": 15,
     *   "current": "2019-11-00T00:00:00"
     * }
     * ```
     */
    struct [[eosio::table("settings")]] settings_row {
        uint64_t                    vote_margin;
        eosio::time_point_sec       current;
    };

    // Tables
    typedef eosio::multi_index< "proposal"_n, proposal_row> proposal_table;
    typedef eosio::multi_index< "votes"_n, votes_row> votes_table;
    typedef eosio::singleton< "settings"_n, settings_row> settings_table;

    // private helpers
    void check_json( const string json );
};

}