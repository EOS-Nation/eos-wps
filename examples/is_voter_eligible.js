const { JsonRpc } = require("eosjs");
const fetch = require("node-fetch");

// latest eosjs
const endpoint = "https://api.jungle.alohaeos.com";
const rpc = new JsonRpc(endpoint, { fetch });

// main
(async () => {
    const voter = "eosnationftw";
    const gstate = await get_global_table();
    const producer = await get_producer( voter );

    console.log( is_voter_eligible( producer, gstate ) );
    //=> true/false
})();

async function get_global_table()
{
    // variables
    const code = "eosio";
    const scope = "eosio";
    const table = "global";

    // fetch data
    const results = await rpc.get_table_rows({json: true, code, scope, table })

    // log data
    if ( results.rows.length ) return results.rows[0];
    else throw new Error("cannot retrieve global table");
}

async function get_producer( producer )
{
    // variables
    const code = "eosio";
    const scope = "eosio";
    const table = "producers";

    // fetch data
    const results = await rpc.get_table_rows({json: true, code, scope, table, lower_bound: producer, upper_bound: producer })

    // log data
    if ( results.rows.length ) return results.rows[0];
    return null;
}

function calculate_producer_per_vote_pay( total_votes, gstate )
{
    return Number(( gstate.pervote_bucket * total_votes ) / gstate.total_producer_vote_weight );
}

function is_voter_eligible( producer, gstate )
{
    if ( !producer ) return false;

    const producer_per_vote_pay = calculate_producer_per_vote_pay( producer.total_votes, gstate );
    if ( producer_per_vote_pay < 1000000 ) return false;
    return true;
}