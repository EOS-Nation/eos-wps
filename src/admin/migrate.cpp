void wps::migrate( const name type )
{
    require_auth( get_self() );

    if (type == "del.comment"_n) {
        for ( auto row : _proposals ) {
            comments_table _comments( get_self(), row.proposal_name.value );
            auto comment_itr = _comments.begin();
            while ( comment_itr != _comments.end() ) {
                comment_itr = _comments.erase( comment_itr );
            }
        }
    }
}