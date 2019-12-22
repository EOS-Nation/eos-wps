#!/bin/bash

ENDPOINT="https://jungle.eosn.io"
CONTRACT="wpswpswpswps"

function cleos() { command cleos --verbose --url=${ENDPOINT} "$@"; echo $@; }


# propose draft
cleos push action $CONTRACT submitdraft '["deniscarrier", "mywps", "My WPS", "1000.0000 EOS", 2, [{"key":"category", "value":"other"}, {"key":"region", "value":"global"}]]' -p deniscarrier

# deposit EOS into account
cleos transfer deniscarrier $CONTRACT "100.0000 EOS" ""

# activate
cleos push action $CONTRACT activate '["deniscarrier", "mywps"]' -p deniscarrier

# vote
cleos push action $CONTRACT vote '["deniscarrier", "mywps", "yes"]' -p deniscarrier

# vote
cleos push action $CONTRACT setproposer '["deniscarrier", [{"key":"region", "value":"global"}]]' -p deniscarrier

# # init
# cleos push action $CONTRACT init '["2019-12-12T00:00:00"]' -p $CONTRACT

# # setparams (optional)
# cleos push action $CONTRACT setparams '[{"vote_margin": 15, "deposit_required": "100.0000 EOS", "voting_interval": 2592000, "max_monthly_budget": "50000.0000 EOS"}]' -p $CONTRACT

# # setstate (TESTING PURPOSES)
# cleos push action $CONTRACT setstate '[{"current_voting_period": "2019-12-12T00:00:00", "next_voting_period": "2020-01-11T00:00:00" "liquid_deposits": "100.0000 EOS", "locked_deposits": "500.0000 EOS", "available_funding": "150.0000 EOS"}]' -p $CONTRACT

# # cancel draft
# cleos push action $CONTRACT canceldraft '["deniscarrier", "mywps"]' -p deniscarrier

# # modify draft
# cleos push action $CONTRACT modifydraft '["deniscarrier", "mywps", "My WPS", [{"key":"category", "value":"other"}]]' -p deniscarrier

# # refund
# cleos push action $CONTRACT refund '["deniscarrier"]' -p deniscarrier

# # fund wps
# cleos transfer deniscarrier $CONTRACT "50.0000 EOS" "funding"

# # cancel
# cleos push action $CONTRACT canceldraft '["deniscarrier", "mywps"]' -p deniscarrier