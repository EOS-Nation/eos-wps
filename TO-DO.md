## To-Dos

- build using RC's


## major changes

- remove `end` params from `proposals`
- add `remaining_periods` integer in `proposals` (ex: number of voting periods requested)
- when proposal `activate` they start as `active` and -1 from their requested duration
- when proposal `activate` NEXT, their proposal is set to `pending` and `remaining_periods` == `duration`
- whenever `complete` is executed, all `active` proposals will reduce -1 the `remaining_periods`, `pending` proposals are set to `active` using the requested duration.
- any proposals that have 0 are set to either `completed` or `expired` (not enough votes)
- don't add future or past proposals to `periods`
- when `complete` add all `active` proposal names to `periods`
- only include `pending` proposals to `periods` when active
- set `start_voting_period` = 0 for `pending` proposals

## Optional

- limit size of `proposal_json`
- `prune` remove all inactive proposals:
    - clear `expired` proposal
    - clear `completed` or `partial` proposals once `payouts` & `claimed` equals 0 EOS
- add `clearvotes` admin action (removal of all votes)
