#!/bin/bash

# Config Constants

CYAN='\033[1;36m'
GREEN='\033[0;32m'
NC='\033[0m'

# load default key into local wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)
cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

# activate PREACTIVATE_FEATURE
echo -e "${CYAN}-----------------------PREACTIVATE FEATURE-----------------------${NC}"
curl -X POST http://127.0.0.1:8888/v1/producer/schedule_protocol_feature_activations -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}' | jq

# create system accounts
echo -e "${CYAN}-----------------------CREATING SYSTEM ACCOUNTS-----------------------${NC}"
cleos create account eosio eosio.bpay EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.vpay EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.saving EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.names EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.stake EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.rex EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.ram EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.ramfee EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# deploy smart contract
echo -e "${CYAN}-----------------------DEPLOYING CONTRACTS-----------------------${NC}"
cleos set contract eosio ./external/eosio.system eosio.system.wasm eosio.system.abi
cleos set contract eosio.token ./external/eosio.token eosio.token.wasm eosio.token.abi

# create token
echo -e "${CYAN}-----------------------CREATE EOS TOKEN-----------------------${NC}"
cleos push action eosio.token create '["eosio", "100000000000.0000 EOS"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "1000000000.0000 EOS", "init"]' -p eosio

# init
cleos push action eosio init '[0, "4,EOS"]' -p eosio

# create accounts
echo -e "${CYAN}-----------------------CREATE ACCOUNTS-----------------------${NC}"
cleos system newaccount eosio voter EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "75000000.0000 EOS" --stake-net "75000000.0000 EOS" --buy-ram-kbytes 8 --transfer
cleos system newaccount eosio smallvoter EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "30000000.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 8 --transfer
cleos system newaccount eosio myaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 8 --transfer
cleos system newaccount eosio toaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 8 --transfer
cleos system newaccount eosio mybp1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 8 --transfer
cleos system newaccount eosio mybp2 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 8 --transfer
cleos system newaccount eosio mybp3 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 8 --transfer
cleos system newaccount eosio mybp4 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 8 --transfer
cleos system newaccount eosio mybp5 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 8 --transfer

# transfer tokens
cleos transfer eosio myaccount "2000.0000 EOS"
cleos transfer eosio toaccount "2000.0000 EOS"
cleos transfer eosio eosio.names "100000.0000 EOS"

# WPS Contract
cleos system newaccount eosio eosio.wps EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-bytes 2097152  --transfer
cleos set account permission eosio.wps active --add-code
cleos set contract eosio.wps ./dist eosio.wps.wasm eosio.wps.abi

# regproducer BP's
cleos system regproducer mybp1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos system regproducer mybp2 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos system regproducer mybp3 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos system regproducer mybp4 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos system regproducer mybp5 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# vote for BP's
cleos system voteproducer prods voter mybp1 mybp2 mybp3 mybp4
cleos system voteproducer prods smallvoter mybp5

sleep 1
# claimrewards (custom system contract)
# pervote_bucket = 227746253
# total_producer_vote_weight = 139063550990300872704.00000000000000000
cleos system claimrewards mybp1
