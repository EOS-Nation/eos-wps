const { JsonRpc } = require("eosjs");
const fetch = require("node-fetch");

// latest eosjs
const endpoint = "http://localhost:8888";
const rpc = new JsonRpc(endpoint, { fetch });

// main
(async () => {
  // variables
  const code = "eosio.wps";
  const scope = "eosio.wps";
  const proposer = "toaccount";
  const table = "proposals";
  const index_position = 3; // proposer multi-index
  const key_type = "i64";

  // fetch data
  const results = await rpc.get_table_rows({json: true, code, scope, table, index_position, key_type, lower_bound: proposer, upper_bound: proposer })

  // log data
  for (const row of results.rows) {
    console.log(row)
  }
})();