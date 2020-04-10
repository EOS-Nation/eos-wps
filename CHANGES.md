# 2020-04-10

- update `copy_active_voting_periods` to only copy over `active` proposals
- fix `copy_active_voting_periods` & `add_proposal_to_periods` to match the same next voting period

# 2020-04-09

**BREAKING** changes:
1. renamed all map<name, string> types:
- `comment_json`
- `proposer_json`
- `proposal_json` (no change)

2. actions also affected by renaming:
- `setproposer`
- `comment`

3. ACTION `activate` has modified parameters (`start_voting_period` replaced by `bool activate_next`)
> true = activate next
> false = active current

**NEW** features:
- `account_type` is added to comments TABLE (multi-index 2)
> 3 types: voter, proposer, other

- `remaining_voting_periods` has been added to `proposals` TABLE (counts to track how many voting periods is left for the proposals)

# 2020-04-02 to 2020-04-09

### SECURITY AUDIT FIXES

- add `check_wps_parameters` to for both `init` & `setparams`:
  - `voting_interval` must be equal to 30 days (2592000)
  - `max_monthly_budget` must be postive asset
  - `deposit_required` must be positive asset
  - `vote_margin` must be above zero
- rename `CORE_SYMBOL` => `BUDGET_SYMBOL`
- rename `CORE_TOKEN_CONTRACT` => `BUDGET_TOKEN_CONTRACT`
- to be eligible to comment, must be `proposer` or `voter` must cast a vote on that particular proposal.
- `min_time_voting_end` must less than `voting_interval`
- remove unused parameters
- remove `update_eligible_proposals()` executing a second time at `complete`
- fix `set_pending_to_active()` to modify multiple rows
- add explicite check for to prevent having above 100 active proposals
- retain ineligible votes
  - do not delete votes when voter becomes ineligible in `refresh_proposal()`
  - handle ineligible producers in `calculate_total_net_votes()`
  - simplify `refresh_proposal()`
  - allows voters to `comment()` if no longer eligible
- add `calculate_producer_per_vote_pay` helper method

#### `remaining_voting_periods` major change

- remove `proposals::end` value
- add `proposals::remaining_voting_periods` number which represent how many remaining of voting periods the proposal has.
- when proposal `activate` the `remaining_voting_periods` is set to the `duration` (for both active & pending proposals)
- whenever `complete` is executed, all `active` proposals will reduce -1 the `remaining_voting_periods`, `pending` proposals are unchanged.
- any proposals that have 0 duration are set to either `completed`/`expired`/`partial`
- replace `start_voting_period` from `activate()` and replace with `bool activate_next`
- don't add future or past proposals to `periods`
- when `complete` add all `active` & `pending` proposal names to `periods`
- only include `pending` proposals to `periods` when active
- set `start_voting_period` = 0 for `pending` proposals
- when `complete` update current & next voting periods to 0:00 UTC (in case multiple days/weeks are skipped, the current voting period will be set to the day the last `complete` is executed).
- set `start_voting_period` for proposals changing from `pending` => `active`

#### Breaking UI changes

- replace `start_voting_period` from `activate()` ACTION and replace with `bool activate_next` (true = activate next, false = active current)
- replace `end` value with `remaining_voting_periods` in `proposals` TABLE
- `remaining_voting_periods` number represents how many remaining of voting periods the proposal has.

# 2020-03-29

- add `CORE_TOKEN_CONTRACT` to replace hardcoded value of `eosio.token`

# 2020-03-28

- add check if proposer is eligible to activate proposal

# 2020-03-17

- fix `check_voter_eligible()` functionality
- remove TESTING related actions

# 2020-03-17

- removed deferred transaction from `complete`
- remove `pause` param in settings
- `vote` added check for `active` proposals
- if proposal doesn't met threshold, set `status` to `expired` (cancels all subsequent periods for proposal)
- re-order `on_notify::transfer` to allow donation deposits prior to contract being initialized

# 2020-03-14

- add `wps_parameters` to `init()`
- merge `start()` => `init()`
- `refresh` deletes votes instead of setting them as "ignore"
- cannot activate proposals during insolvent voting period

# 2020-03-11

- remove `require_auth(get_self())` from `start` ACTION

# 2020-03-10

- add `refresh()` ACTION to re-calculate votes from a particular voter
  - check if voter is still eligible as a voter
  - set vote to `ignore` if not eligible and voting on proposals
  - re-calculate total net votes & eligible proposals
- removed last 24 hour claim requirement ( already being solved by minimum of 100 EOS requirement )

# 2020-03-09

- Add support to add deposit from alternative account
  - Send EOS token transfer using `memo` of deposit account
- change default vote margin to 20
- change default minimum time required to activate to 5 days
- when activating proposal, deposit amount gets allocated towards `state::available_funding`
  - `locked_deposits` is informational data (not being used by contract)
- `start()` initiates at 00:00 UTC
- added `check_available_funding()` to prevent executing `start` & `complete` due to lack of funds
- enforce `vote` for producers to a minimum of 100 EOS as vote total
  - vote errors:
    - not claimed for the last 24 hours
    - not enough 100 EOS votes
    - nothing changed
- added `pause` functionality
  - **disabled**:  `complete()`, `start()` & incoming prevent deposits
  - **available**: `refund()` & `claim()` & `submitdraft()`

# 2020-02-22

- add `CORE_SYMBOL` ("EOS,4")
- add `start` ACTION to initiate voting periods
- modify `init` ACTION to only initialize `state` & `settings` paramaters (other logic has been moved to `start` action)

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

