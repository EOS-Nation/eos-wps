#include <eosio.wps/eosio.wps.hpp>

// utils
#include "./utils/get_tx_id.cpp"
#include "./utils/deferred.cpp"

// testing
#include "./testing/transfers.cpp"
#include "./testing/setstate.cpp"
#include "./testing/clean.cpp"

// main
#include "drafts.cpp"
#include "on_notify.cpp"
#include "activate.cpp"
#include "deposits.cpp"
#include "periods.cpp"
#include "settings.cpp"
#include "vote.cpp"
#include "proposers.cpp"
#include "complete.cpp"
