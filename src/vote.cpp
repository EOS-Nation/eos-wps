namespace eosio {

void wps::vote( const eosio::name voter, const eosio::name proposal_name, const eosio::name vote )
{
    require_auth( voter );

    auto proposals_itr = _proposals.find( proposal_name.value );
    check( proposals_itr != _proposals.end(), "[proposals::proposal_name] does not exists");
    check( proposals_itr->status == "active"_n, "[proposals::proposal_name] is not active");

    auto votes_itr = _votes.find( proposal_name.value );
    check( votes_itr != _votes.end(), "[votes::proposal_name] does not exist");
    check( vote == "yes"_n || vote == "no"_n || vote == "abstain"_n || vote == ""_n, "[vote] invalid (ex: yes/no/abstain)");
    check( votes_itr->end > current_time_point(), "proposal has ended");

    _votes.modify( votes_itr, voter, [&]( auto& row ) {
        check(row.votes[voter] != vote, "[vote] has not been modified");

        if (vote == ""_n) row.votes.erase(voter); // unvote
        else row.votes[voter] = vote; // vote

        // re-caculate votes
        row.total_net_votes = calculate_total_net_votes( row.votes );
    });
}

int16_t wps::calculate_total_net_votes( const std::map<eosio::name, eosio::name> votes )
{
    int16_t total_net_votes = 0;
    for (std::pair<eosio::name, eosio::name> item : votes) {
        const eosio::name vote = item.second;
        if (vote == "yes"_n) total_net_votes += 1;
        else if (vote == "no"_n) total_net_votes -= 1;
    }
    return total_net_votes;
}

}