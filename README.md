# EOSIO WPS

## Workflows

### A. Propose & Activate

1. `propose` proposal draft
2. send 100 EOS from any account using memo as the proposal name
3. `activate` (can no longer be modified)

### B. Cancel draft

1. `propose` proposal draft
2. `canceldraft`

### C. Cancel & Refund

1. `propose` proposal draft
2. send 100 EOS
3. `refund` 100 EOS
4. `canceldraft`

## ACTION - USER

- [`propose`](#action-propose)
- [`vote`](#action-vote)
- [`activate`](#action-activate)
- [`refund`](#action-refund)
- [`canceldraft`](#action-canceldraft)
- [`modifydraft`](#action-modifydraft)

## ACTION - ADMIN

- [`setparams`](#action-setparams)
- [`init`](#action-init)

## TABLE

- [`draft`](#table-draft)
- [`proposal`](#table-proposal)

## ACTION `propose`

Submit a WPS proposal

- Authority:  `proposer`

### params

- `{name} proposer` - proposer of proposal
- `{name} proposal_name` - proposal name
- `{string} title` - proposal title
- `{map<name, string>} proposal_json` - proposal JSON metadata
- `{asset} budget` - monthly budget payment request
- `{uin8_t} duration` - monthly budget duration (maximum of 6 months)

### example

```bash
cleos push action eosio.wps propose '["myaccount", "mywps", "My WPS", "500.0000 EOS", 1, [{"key":"category", "value":"other"}, {"key":"region", "value":"global"}]]' -p myaccount
```

## ACTION `vote`

Vote for a WPS proposal

- Authority:  `voter`

- `{name} voter` - voter
- `{name} proposal_name` - proposal name
- `{name} vote` - vote (yes/no/abstain)

```bash
cleos push action eosio.wps vote '["myaccount", "mywps", "yes"]' -p myaccount
```

## ACTION `activate`

Activate WPS proposal

- Authority:  `proposer`

- `{name} proposer` - proposer
- `{name} proposal_name` - proposal name

```bash
cleos push action eosio.wps activate '["myaccount", "mywps"]' -p myaccount
```

## ACTION `refund`

Refund any remaining deposit amount from a draft WPS proposal.

- Authority:  `proposer`

- `{name} proposer` - proposer
- `{name} proposal_name` - proposal name

```bash
cleos push action eosio.wps refund '["myaccount", "mywps"]' -p myaccount
```

## ACTION `canceldraft`

Cancel draft WPS proposal

- Authority:  `proposer`

- `{name} proposer` - proposer
- `{name} proposal_name` - proposal name

```bash
cleos push action eosio.wps canceldraft '["myaccount", "mywps"]' -p myaccount
```

## ACTION `modifydraft`

Modify draft WPS proposal

- Authority:  `proposer`

### params

- `{name} proposer` - proposer of proposal
- `{name} proposal_name` - proposal name
- `{string} title` - proposal title
- `{map<name, string>} proposal_json` - a sorted container of <key, value>

### example

```bash
cleos push action eosio.wps modifydraft '["myaccount", "mywps", "My WPS", [{"key":"region", value":"global"}]]' -p myaccount
```

## ACTION `init`

Initialize WPS contract

- Authority:  `get_self()`

- `{time_point_sec} initial_voting_period` - initial voting period

```bash
cleos push action eosio.wps init '["2019-11-25T00:00:00"]' -p eosio.wps
```

## ACTION `setparams`

Set paramaters for WPS contract

- Authority:  `get_self()`

- `{int16_t} [vote_margin=15]` - minimum BP vote margin threshold to reach for proposals
- `{asset} [deposit_required="100.0000 EOS"]` - deposit required to active proposal
- `{uint64_t} [voting_interval=2592000]` -  election interval in seconds
- `{asset} [max_monthly_budget="50000.0000 EOS"]` - maximum monthly budget

```bash
cleos push action eosio.wps setparams '[{"vote_margin": 15, "deposit_required": "100.0000 EOS", "voting_interval": 2592000, "max_monthly_budget": "50000.0000 EOS"}]' -p eosio.wps
```

## TABLE `proposals`

- `{name} proposer` - proposer of proposal
- `{name} proposal_name` - proposal name
- `{string} title` - proposal title
- `{asset} budget` - monthly budget payment request
- `{uint8_t} duration` - monthly budget duration (maximum of 6 months)
- `{name} status` - current status of proposal (draft/active/completed/expired)
- `{asset} deposit` - deposit required to active proposal
- `{asset} payments` - payments made to WPS proposal
- `{map<name, string>} proposal_json` - a sorted container of <key, value>

### example

```json
{
  "proposer": "myaccount",
  "proposal_name": "mywps",
  "title": "My WPS",
  "budget": "500.0000 EOS",
  "duration": 1,
  "status": "draft",
  "deposit": "0.0000 EOS",
  "payments": "0.0000 EOS",
  "proposal_json": [
    { "key": "category", "value": "other" },
    { "key": "region", "value": "global" }
  ]
}
```

## TABLE `votes`

- `{name} proposal_name` - The proposal's name, its ID among all proposals
- `{int16_t} total_net_votes` - total net votes
- `{time_point_sec} voting_period` - active voting period (UTC)
- `{time_point_sec} start` - start of voting period (UTC)
- `{time_point_sec} end` - end of voting period (UTC)
- `{map<name, name>} votes` - a sorted container of <voter, vote>

### example

```json
{
  "proposal_name": "mywps",
  "total_net_votes": 2,
  "voting_period": "2019-11-01T00:00:00",
  "start": "2019-11-05T12:10:00",
  "end": "2019-12-01T00:00:00",
  "votes": [
      { "key": "mybp1", "value": "yes" },
      { "key": "mybp2", "value": "no" },
      { "key": "mybp3", "value": "yes" },
      { "key": "mybp4", "value": "abstain" },
      { "key": "mybp5", "value": "yes" }
  ]
}
```

## TABLE `settings`

- `{int16_t} [vote_margin=15]` - minimum BP vote margin threshold to reach for proposals
- `{asset} [deposit_required="100.0000 EOS"]` - deposit required to active proposal
- `{uint64_t} [voting_interval=2592000]` -  election interval in seconds
- `{asset} [max_monthly_budget="50000.0000 EOS"]` - maximum monthly budget

### example

```json
{
  "vote_margin": 15,
  "deposit_required": "100.0000 EOS",
  "voting_interval": 2592000,
  "max_monthly_budget": "50000.0000 EOS"
}
```

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