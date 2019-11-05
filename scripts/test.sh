#!/bin/bash

cleos push action eosio.wps '["myaccount", "mywps", "My WPS", "{\"category\": \"other\", \"region\": \"global\"}", "500.0000 EOS", 1]' -p myaccount
