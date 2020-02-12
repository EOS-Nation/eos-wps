void wps::update_producer( const eosio::name producer )
{
    const name ram_payer = get_self();
    const auto producers_itr = _producers.find( producer.value );

    if ( producers_itr == _producers.end() ) {
        _producers.emplace( ram_payer, [&]( auto& row ) {
            row.producer = producer;
            row.eligible = true;
            row.total_votes = 1000000;
            row.last_claim_time = current_time_point();
        });
    } else {
        _producers.modify( producers_itr, same_payer, [&]( auto& row ) {
            row.last_claim_time = current_time_point();
        });
    }
}
