#!/bin/bash


# init, fund & start
cleos transfer eosio.names eosio.wps "25000.0000 EOS"
cleos -v push action eosio.wps init '[{"vote_margin": 2, "deposit_required": "100.0000 EOS", "voting_interval": 86400, "max_monthly_budget": "25000.0000 EOS", "min_time_voting_end": 0 }]' -p eosio.wps

# setparams
# 1 day   = 86400
# 30 days = 2592000
# cleos push action eosio.wps setparams '[{"vote_margin": 2, "deposit_required": "100.0000 EOS", "voting_interval": 86400, "max_monthly_budget": "25000.0000 EOS", "min_time_voting_end": 3600 }]' -p eosio.wps

# set proposer
cleos -v push action eosio.wps setproposer '["myaccount", [{"key":"region", "value":"global"}]]' -p myaccount

# propose draft
cleos -v push action eosio.wps submitdraft '["myaccount", "mywps", "My WPS", "500.0000 EOS", 3, [{"key":"region", "value":"global"}]]' -p myaccount
cleos -v push action eosio.wps submitdraft '["toaccount", "towps", "To WPS", "200.0000 EOS", 6, [{"key":"category", "value":"other"}]]' -p toaccount
cleos -v push action eosio.wps submitdraft '["toaccount", "short", "Short WPS", "800.0000 EOS", 1, [{"key":"category", "value":"short"}]]' -p toaccount
cleos -v push action eosio.wps submitdraft '["toaccount", "novote", "No Vote WPS", "500.0000 EOS", 1, [{"key":"category", "value":"novote"}]]' -p toaccount
cleos -v push action eosio.wps submitdraft '["toaccount", "optional", "Optional Vote WPS", "500.0000 EOS", 1, [{"key":"category", "value":"optional"}]]' -p toaccount
cleos -v push action eosio.wps submitdraft '["myaccount", "large", "My WPS", "24500.0000 EOS", 1, [{"key":"category", "value":"large"}]]' -p myaccount

# # cancel draft
# cleos -v push action eosio.wps canceldraft '["myaccount", "mywps"]' -p myaccount

# # modify draft
# cleos -v push action eosio.wps modifydraft '["myaccount", "mywps", "My WPS", [{"key":"category", "value":"other"}]]' -p myaccount

# deposit EOS into account
cleos -v transfer myaccount eosio.wps "200.0000 EOS" ""
cleos -v transfer toaccount eosio.wps "400.0000 EOS" ""

# # refund
# cleos -v push action eosio.wps refund '["myaccount"]' -p myaccount

# activate
cleos -v push action eosio.wps activate '["myaccount", "mywps", null]' -p myaccount
cleos -v push action eosio.wps activate '["toaccount", "towps", null]' -p toaccount
cleos -v push action eosio.wps activate '["toaccount", "short", null]' -p toaccount
cleos -v push action eosio.wps activate '["toaccount", "novote", null]' -p toaccount
cleos -v push action eosio.wps activate '["myaccount", "large", null]' -p myaccount

# # cancel
# cleos -v push action eosio.wps canceldraft '["myaccount", "mywps"]' -p myaccount

# vote
cleos -v push action eosio.wps vote '["mybp1", "mywps", "yes"]' -p mybp1
cleos -v push action eosio.wps vote '["mybp2", "mywps", "yes"]' -p mybp2
cleos -v push action eosio.wps vote '["mybp3", "mywps", "yes"]' -p mybp3
cleos -v push action eosio.wps vote '["mybp4", "mywps", "yes"]' -p mybp4

cleos -v push action eosio.wps vote '["mybp1", "towps", "yes"]' -p mybp1
cleos -v push action eosio.wps vote '["mybp2", "towps", "yes"]' -p mybp2
cleos -v push action eosio.wps vote '["mybp3", "towps", "yes"]' -p mybp3
cleos -v push action eosio.wps vote '["mybp4", "towps", "abstain"]' -p mybp4

cleos -v push action eosio.wps vote '["mybp1", "short", "abstain"]' -p mybp1
cleos -v push action eosio.wps vote '["mybp2", "short", "abstain"]' -p mybp2
cleos -v push action eosio.wps vote '["mybp3", "short", "yes"]' -p mybp3
cleos -v push action eosio.wps vote '["mybp4", "short", "yes"]' -p mybp4

cleos -v push action eosio.wps vote '["mybp1", "large", "yes"]' -p mybp1
cleos -v push action eosio.wps vote '["mybp2", "large", "yes"]' -p mybp2
cleos -v push action eosio.wps vote '["mybp3", "large", "yes"]' -p mybp3
cleos -v push action eosio.wps vote '["mybp4", "large", "yes"]' -p mybp4

cleos -v push action eosio.wps vote '["mybp1", "novote", "abstain"]' -p mybp1
cleos -v push action eosio.wps vote '["mybp2", "novote", "no"]' -p mybp2
cleos -v push action eosio.wps vote '["mybp3", "novote", "abstain"]' -p mybp3
cleos -v push action eosio.wps vote '["mybp4", "novote", "no"]' -p mybp4

# can't vote
cleos -v push action eosio.wps vote '["mybp5", "mywps", "yes"]' -p mybp5

# change bp vote & refresh
cleos system voteproducer prods voter mybp1 mybp2 mybp3
cleos push action eosio.wps refresh '[]' -p myaccount

# # complete
# cleos push action eosio.wps complete '[]' -p eosio.wps

