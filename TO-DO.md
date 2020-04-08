## To-Dos

- build using RC's

## major changes

- [x] remove `end` params from `proposals`
- [x] add `remaining_voting_periods` integer in `proposals` (ex: number of voting periods requested)
- [x] when proposal `activate` the `remaining_voting_periods` is set to the `duration` (for both active & pending proposals)
- [ ] whenever `complete` is executed, all `active` proposals will reduce -1 the `remaining_voting_periods`, `pending` proposals are unchanged.
- [x] any proposals that have 0 duration are set to either `completed`/`expired`/`partial`
- [ ] don't add future or past proposals to `periods`
- [ ] when `complete` add all `active` proposal names to `periods`
- [ ] only include `pending` proposals to `periods` when active
- [ ] set `start_voting_period` = 0 for `pending` proposals
- [ ] when `complete` update current & next voting periods to 0:00 UTC (in case multiple days/weeks are skipped, the current voting period will be set to the day the last `complete` is executed).
- [ ] add date/hour/min to when `complete` is finished

## Optional

- limit size of `proposal_json`
- `prune` remove all inactive proposals:
    - clear `expired` proposal
    - clear `completed` or `partial` proposals once `payouts` & `claimed` equals 0 EOS
- add `clearvotes` admin action (removal of all votes)
