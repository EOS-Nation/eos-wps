## To-Dos

- build using RC's
- removal of `claims`
- test `check_voter_eligible()` functionality
- `prune` remove all inactive proposals:
    - clear `expired` proposal
    - clear `completed` or `partial` proposals once `payouts` & `claimed` equals 0 EOS
- add comments
    - add `comment( account, proposal_name, comment_json )` ACTION
    - add `comments` TABLE
- add `clearvotes` admin action (removal of all votes)
