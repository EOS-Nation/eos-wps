/**
 * TESTING ONLY
 *
 * Should be removed in production
 */
void wps::clean( const eosio::name table, const std::optional<eosio::name> scope )
{
    require_auth( get_self() );

    // votes
    if (table == "votes"_n) {
        auto votes_itr = _votes.begin();
        while ( votes_itr != _votes.end() ) {
            votes_itr = _votes.erase(votes_itr);
        }
    // proposals
    } else if (table == "proposals"_n) {
        auto proposals_itr = _proposals.begin();
        while ( proposals_itr != _proposals.end() ) {
            proposals_itr = _proposals.erase( proposals_itr );
        }
    }
    else if (table == "settings"_n) _settings.remove();
    else if (table == "state"_n) _state.remove();
}
