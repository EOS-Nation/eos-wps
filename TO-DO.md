## To-Dos

- build using RC's

## major changes

- [x] remove `end` params from `proposals`
- [x] add `remaining_voting_periods` integer in `proposals` (ex: number of voting periods requested)
- [x] when proposal `activate` the `remaining_voting_periods` is set to the `duration` (for both active & pending proposals)
- [ ] whenever `complete` is executed, all `active` proposals will reduce -1 the `remaining_voting_periods`, `pending` proposals are unchanged.
- [ ] any proposals that have 0 are set to either `completed` or `expired` (not enough votes)
- [ ] don't add future or past proposals to `periods`
- [ ] when `complete` add all `active` proposal names to `periods`
- [ ] only include `pending` proposals to `periods` when active
- [ ] set `start_voting_period` = 0 for `pending` proposals

## Optional

- limit size of `proposal_json`
- `prune` remove all inactive proposals:
    - clear `expired` proposal
    - clear `completed` or `partial` proposals once `payouts` & `claimed` equals 0 EOS
- add `clearvotes` admin action (removal of all votes)
