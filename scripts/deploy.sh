#!/bin/bash

# Config Constants

CYAN='\033[1;36m'
GREEN='\033[0;32m'
NC='\033[0m'

# load default key into local wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)
cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

# create accounts
echo -e "${CYAN}-----------------------CREATING ACCOUNTS-----------------------${NC}"
cleos create account eosio eosio.wps EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.names EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.ramfee EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio myaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio toaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio mybp1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio mybp2 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio mybp3 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio mybp4 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio mybp5 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set account permission eosio.wps active --add-code

# deploy smart contract
echo -e "${CYAN}-----------------------DEPLOYING CONTRACTS-----------------------${NC}"
cleos set contract eosio.wps ./dist eosio.wps.wasm eosio.wps.abi
cleos set contract eosio.token ./dist eosio.token.wasm eosio.token.abi
cleos set contract eosio ./dist eosio.system.wasm eosio.system.abi

# create token
echo -e "${CYAN}-----------------------CREATE EOS TOKEN-----------------------${NC}"
cleos push action eosio.token create '["eosio", "1000000000.0000 EOS"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "1000000000.0000 EOS", "init"]' -p eosio
cleos transfer eosio myaccount "1000.0000 EOS"
cleos transfer eosio toaccount "1000.0000 EOS"
cleos transfer eosio eosio.names "50000.0000 EOS"

echo -e "${GREEN}--> Done${NC}"

# setproducer (LOCAL ONLY)
cleos -v push action eosio setproducer '["mybp1", 3997400451565694464]' -p eosio
cleos -v push action eosio setproducer '["mybp2", 3997400451565694464]' -p eosio
cleos -v push action eosio setproducer '["mybp3", 3997400451565694464]' -p eosio
cleos -v push action eosio setproducer '["mybp4", 3997400451565694464]' -p eosio
cleos -v push action eosio setproducer '["mybp5", 3997400451565694464]' -p eosio