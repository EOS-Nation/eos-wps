#!/usr/bin/env bash

mkdir -p dist

GREEN='\033[0;32m'
NC='\033[0m'

# eosio.wps
echo -e "${GREEN}Compiling eosio.wps...${NC}"
eosio-cpp -abigen \
  ./src/eosio.wps.cpp \
  -contract eosio.wps \
  -R ./ricardian \
  -o ./dist/eosio.wps.wasm \
  -I ./include \
  -I ./external/eosio.token/include \
  -I ./external/eosio.system/include

# # eosio.token
# echo -e "${GREEN}Compiling eosio.token...${NC}"
# eosio-cpp -abigen \
#   external/eosio.token/src/eosio.token.cpp \
#   -contract eosio.token \
#   -o ./dist/eosio.token.wasm \
#   -I external/eosio.token/include
