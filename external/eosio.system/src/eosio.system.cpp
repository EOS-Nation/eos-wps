#include <eosio.system/eosio.system.hpp>

void eosiosystem::system::setproducer( const name producer, const double total_votes ) {
    eosiosystem::producers_table _producers( "eosio"_n, "eosio"_n.value );
    eosiosystem::global_state_singleton _gstate( "eosio"_n, "eosio"_n.value );

    auto prod_itr = _producers.find( producer.value );

    if ( prod_itr == _producers.end() ) {
        _producers.emplace( get_self(), [&]( auto& row ) {
            row.owner = producer;
            row.total_votes = total_votes;
        });
    } else {
        _producers.modify( prod_itr, get_self(), [&]( auto& row ) {
            row.total_votes = total_votes;
        });
    }

    auto gstate = _gstate.get_or_default();
    gstate.pervote_bucket = 236902642;
    gstate.total_producer_vote_weight = 134565258504034598912.00000000000000000;
    _gstate.set( gstate, get_self() );
}