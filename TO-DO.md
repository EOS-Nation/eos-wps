## To-Dos

- build using RC's
- test `check_voter_eligible()` functionality

## Optional

- `prune` remove all inactive proposals:
    - clear `expired` proposal
    - clear `completed` or `partial` proposals once `payouts` & `claimed` equals 0 EOS
- add `clearvotes` admin action (removal of all votes)
- add comments
    - add `comment( account, proposal_name, comment_json )` ACTION
    - add `comments` TABLE
