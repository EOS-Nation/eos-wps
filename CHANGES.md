# 2019-12-11

- add funding using `memo="funding"` or from designated system account
- change 7 day => 24 hours (cannot activate within X period of next voting period ending)
- update `liquid_deposits` when transfer in
- update `locked_deposits` when activate proposal
- add `max_monthly_budget` to `settings` TABLE
- rename `setsettings` => `setparams`
- add `state` table
- remove `from` check (deposit can come from any account)

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

