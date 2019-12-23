#!/bin/bash

# init
cleos -v push action eosio.wps init '["2019-11-25T00:00:00"]' -p eosio.wps

# setparams (optional)
cleos push action eosio.wps setparams '[{"vote_margin": 15, "deposit_required": "100.0000 EOS", "voting_interval": 2592000, "max_monthly_budget": "50000.0000 EOS"}]' -p eosio.wps

# set proposer
cleos -v push action eosio.wps setproposer '["myaccount", [{"key":"region", "value":"global"}]]' -p myaccount

# propose draft
cleos -v push action eosio.wps submitdraft '["myaccount", "mywps", "My WPS", "500.0000 EOS", 2, [{"key":"region", "value":"global"}]]' -p myaccount
cleos -v push action eosio.wps submitdraft '["toaccount", "towps", "To WPS", "100.0000 EOS", 6, [{"key":"category", "value":"other"}]]' -p toaccount

# # cancel draft
# cleos -v push action eosio.wps canceldraft '["myaccount", "mywps"]' -p myaccount

# # modify draft
# cleos -v push action eosio.wps modifydraft '["myaccount", "mywps", "My WPS", [{"key":"category", "value":"other"}]]' -p myaccount

# deposit EOS into account
cleos -v transfer myaccount eosio.wps "100.0000 EOS" ""
cleos -v transfer toaccount eosio.wps "300.0000 EOS" ""

# refund
# cleos -v push action eosio.wps refund '["myaccount"]' -p myaccount

# activate
cleos -v push action eosio.wps activate '["myaccount", "mywps", "2019-11-25T00:00:00"]' -p myaccount
cleos -v push action eosio.wps activate '["toaccount", "towps", "2019-12-25T00:00:00"]' -p toaccount

# fund wps
cleos -v transfer eosio.ramfee eosio.wps "50000.0000 EOS" ""

# cancel
# cleos -v push action eosio.wps canceldraft '["myaccount", "mywps"]' -p myaccount

# vote
cleos -v push action eosio.wps vote '["mybp1", "mywps", "yes"]' -p mybp1
cleos -v push action eosio.wps vote '["mybp2", "mywps", "no"]' -p mybp2
cleos -v push action eosio.wps vote '["mybp3", "mywps", "yes"]' -p mybp3
cleos -v push action eosio.wps vote '["mybp4", "mywps", "abstain"]' -p mybp4
cleos -v push action eosio.wps vote '["mybp5", "mywps", "yes"]' -p mybp5

cleos -v push action eosio.wps vote '["mybp1", "towps", "yes"]' -p mybp1
cleos -v push action eosio.wps vote '["mybp3", "towps", "yes"]' -p mybp3
cleos -v push action eosio.wps vote '["mybp5", "towps", "yes"]' -p mybp5