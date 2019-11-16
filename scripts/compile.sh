#!/usr/bin/env bash

mkdir -p dist

GREEN='\033[0;32m'
NC='\033[0m'
echo -e "${GREEN}Compiling eosio.wps...${NC}"

if [[ $1 == "wps" || $1 == "" ]]
then
  # eosio.wps
  eosio-cpp -abigen \
    contracts/eosio.wps/src/eosio.wps.cpp \
    -contract eosio.wps \
    -R contracts/eosio.wps/ricardian \
    -o ./dist/eosio.wps.wasm \
    -I contracts/eosio.wps/include
fi

if [[ $1 == "token" || $1 == "" ]]
then
  # eosio.token
  eosio-cpp -abigen \
    contracts/eosio.token/src/eosio.token.cpp \
    -contract eosio.token \
    -R contracts/eosio.token/ricardian \
    -o ./dist/eosio.token.wasm \
    -I contracts/eosio.token/include
fi