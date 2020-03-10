#include <eosio.system/eosio.system.hpp>

void eosiosystem::system::setproducer( const eosio::name producer ) {
    eosiosystem::producers_table _producers( "eosio"_n, "eosio"_n.value );
    eosiosystem::global_state_singleton _gstate( "eosio"_n, "eosio"_n.value );

    auto prod_itr = _producers.find( producer.value );
    check( prod_itr == _producers.end(), "producer already exists");

    if ( prod_itr == _producers.end() ) {
        _producers.emplace( get_self(), [&]( auto& row ) {
            row.owner = producer;
            row.total_votes = 3997400451565694464.00000000000000000;
            row.last_claim_time = current_time_point();
        });
    }

    auto gstate = _gstate.get_or_default();
    gstate.pervote_bucket = 236902642;
    gstate.total_producer_vote_weight = 134565258504034598912.00000000000000000;
    _gstate.set( gstate, get_self() );
}