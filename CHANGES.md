# 2020-02-22

- remove `auto-complete` which uses deferred transactions to execute at the next voting period (background script must be used to execute `complete` action).

# 2020-01-16

- update `memo` for `claim` action => `wps::<proposal_name>`

# 2020-01-15

- add `check_voter_eligible` check, must be producer and claimed producer rewards within the last 24 hours
- `voteproducer` action (TESTING PURPOSES) to vote as eligible BP voter

# 2019-12-29

- add `claim` ACTION - to receive claimable amount from proposal to proposer
- add `{asset} proposals::claimable` - available amount to claim
- add `{bool} proposals::eligible` - (true/false) eligible for current voting period payout
- define `voting_period` param as OPTIONAL for `activate` ACTION
- add `claims` TABLE
- drop `transfers` TABLE
- update `eligible` logic on `vote` ACTION
- use `eligible` field on `complete` ACTION

## TO-DO

- add `vote_period` to `complete` ACTION

# 2019-12-28

- rename `proposals::payments` => `proposals::payouts`

# 2019-12-26

- add `pending` status for proposals that use the `activte` next voting period feature
- `complete` action sets all `pending` proposals to `active`

# 2019-12-23

- added `min_time_voting_end` to `settings` TABLE
- added `voting_period` param to `activate` ACTION (must be current or next)
- add `auto_complete` which pushes `complete` deferred transaction using `next_voting_period` as the delay
- add additional check "[proposal_name] is already activated" message when activating a draft for the second time
- renamed `proposals::start` => `proposals::start_voting_period`
- rename `periods::period` => `periods::voting_period`

# 2019-12-22

- add `check_completed` check, cannot perform `activate`/`vote` ACTIONS during completed voting period phase
- add check for "cannot exceed 100 proposals per single voting period"

# 2019-12-21

- added `payments` (eosio::asset) param to `proposals` TABLE
- move `total_net_votes` param from `votes` => `proposals` TABLE
- added `status` (eosio::name) param to `proposals` TABLE
- `vote` ACTION can only use `yes/no/abstain` as `vote` param value
- add secondary index on `proposer` & `status` params for `proposals` TABLE
- add `max_monthly_budget` check when submitting draft
- add `setstate` ACTION (for TESTING purposes)
- add `next_voting_period` param to `state`

# 2019-12-16

- added `modifybudget` ACTION
- define `ram_payer` at begining of action

# 2019-12-13

- add `transfers` TABLE to track incoming funding & deposits transfers (for TESTING purposes)
- 🔥`submitdraft` CHANGE: rename `budget` => `monthly_budget` param
- add `periods` TABLE
- when proposal `activate` add proposal name to `periods` TABLE
- add `total_budget` param

# 2019-12-12

- add check to prevent +13 character `eosio:name` in `proposal_name`
- remove `status` field from `votes` & `proposals`
- add `start` & `end` fields to `proposals`
- simplify `votes` TABLE, only has 3 fields (`proposal_name`, `total_net_votes`, `votes`)
- add `proposers` TABLE & `setproposer` ACTION
- renamed incoming EOS transfer memo "funding" => "donate" (to fund available WPS liquidity)

# 2019-12-12

- rename `propopse` => `submitdraft`
- simplify `refund` action (only have 1 param, "account")
- first time EOS deposits must first require the user to submit a proposal
- new `draft` TABLE scoped by `proposer`
- creating a `proposal` is first sent to `drafts` TABLE scoped under the `proposer` account name
- account can deposit any amount of EOS (regardless of proposal)
- when `activate` proposal, it deducts the amount of EOS directly from the `deposits` account

# 2019-12-11

- add funding using `memo="funding"` or from designated system account
- change 7 day => 24 hours (cannot activate within X period of next voting period ending)
- update `liquid_deposits` when transfer in
- update `locked_deposits` when activate proposal
- add `max_monthly_budget` to `settings` TABLE
- rename `setsettings` => `setparams`
- remove `from` check (deposit can come from any account)
- add `state` table

## TABLE `state`

- `{time_point_sec} current_voting_period` - current voting period
- `{asset} [liquid_deposits="0.0000 EOS"]` - liquid deposits
- `{asset} [locked_deposits="0.0000 EOS"]` - locked deposits
- `{asset} [available_funding="0.0000 EOS"]` - available funding

### example

```json
{
  "current_voting_period": "2019-11-01T00:00:00",
  "liquid_deposits": "100.0000 EOS",
  "locked_deposits": "200.0000 EOS",
  "available_funding": "50000.0000 EOS",
}
```

