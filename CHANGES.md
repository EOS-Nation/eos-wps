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

TO-DO:

- add `proposers` TABLE to add extra metadata about proposer
- add `funding` TABLE to track incoming funding deposits
- handle `vote` on expired proposals

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

