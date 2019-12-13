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
    // deposits
    } else if (table == "deposits"_n) {
        auto deposits_itr = _deposits.begin();
        while ( deposits_itr != _deposits.end() ) {
            deposits_itr = _deposits.erase( deposits_itr );
        }
    // drafts
    } else if (table == "drafts"_n) {
        drafts_table _drafts( get_self(), scope->value );
        auto drafts_itr = _drafts.begin();
        while ( drafts_itr != _drafts.end() ) {
            drafts_itr = _drafts.erase( drafts_itr );
        }
    // transfers
    } else if (table == "transfers"_n) {
        transfers_table _transfers( get_self(), scope->value );
        auto transfers_itr = _transfers.begin();
        while ( transfers_itr != _transfers.end() ) {
            transfers_itr = _transfers.erase( transfers_itr );
        }
    }
    else if (table == "settings"_n) _settings.remove();
    else if (table == "state"_n) _state.remove();
}
