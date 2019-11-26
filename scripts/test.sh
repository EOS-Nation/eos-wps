#!/bin/bash

# init
cleos -v push action eosio.wps init '["2019-11-25T00:00:00"]' -p eosio.wps

# propose
cleos -v push action eosio.wps propose '["myaccount", "mywps", "My WPS", "500.0000 EOS", 1, [{"key":"category", "value":"other"}, {"key":"region", "value":"global"}]]' -p myaccount

# transfer
cleos -v transfer myaccount eosio.wps "100.0000 EOS" "mywps"

# refund
# cleos -v push action eosio.wps refund '["myaccount", "mywps"]' -p myaccount

# activate
cleos -v push action eosio.wps activate '["myaccount", "mywps"]' -p myaccount

# cancel
# cleos -v push action eosio.wps canceldraft '["myaccount", "mywps"]' -p myaccount

# vote
cleos -v push action eosio.wps vote '["mybp1", "mywps", "yes"]' -p mybp1
cleos -v push action eosio.wps vote '["mybp2", "mywps", "no"]' -p mybp2
cleos -v push action eosio.wps vote '["mybp3", "mywps", "yes"]' -p mybp3
cleos -v push action eosio.wps vote '["mybp4", "mywps", "abstain"]' -p mybp4
cleos -v push action eosio.wps vote '["mybp5", "mywps", "yes"]' -p mybp5