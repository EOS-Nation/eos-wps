# EOSIO WPS

## Workflows

### A. Submit & Deposit & Activate

1. `submitdraft` to create a draft proposal
2. send 100 EOS from `proposer` account
3. `activate` proposal, deducts min deposit & can no longer be modified

### B. Submit & Cancel

1. `submitdraft` draft proposal
2. `canceldraft` cancel draft

### C. Deposit & Refund

1. `submitdraft` draft proposal
2. send 100 EOS
3. `refund` action to return 100 EOS

## ACTION - USER

- [`submitdraft`](#action-submitdraft)
- [`modifydraft`](#action-modifydraft)
- [`modifybudget`](#action-modifybudget)
- [`canceldraft`](#action-canceldraft)
- [`vote`](#action-vote)
- [`activate`](#action-activate)
- [`refund`](#action-refund)

## ACTION - ADMIN

- [`setparams`](#action-setparams)
- [`init`](#action-init)

## TABLE

- [`proposals`](#table-proposals)
- [`votes`](#table-votes)
- [`settings`](#table-settings)
- [`state`](#table-state)
- [`deposits`](#table-deposits)
- [`drafts`](#table-drafts)
- [`proposers`](#table-proposers)
- [`periods`](#table-periods)
- [`transfers`](#table-transfers) (TESTING ONLY)

## ACTION `submitdraft`

Submit a draft WPS proposal (create/modify)

- Authority:  `proposer`

### params

- `{name} proposer` - proposer of proposal
- `{name} proposal_name` - proposal name
- `{string} title` - proposal title
- `{asset} monthly_budget` - monthly budget payment request
- `{uin8_t} duration` - monthly budget duration (maximum of 6 months)
- `{map<name, string>} proposal_json` - a sorted container of <key, value>

### example

```bash
cleos push action eosio.wps submitdraft '["myaccount", "mywps", "My WPS", "500.0000 EOS", 1, [{"key":"category", "value":"other"}, {"key":"region", "value":"global"}]]' -p myaccount
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

Refund any remaining deposit amount from requesting account

- Authority:  `account`

- `{name} account` - account requesting refund

```bash
cleos push action eosio.wps refund '["myaccount"]' -p myaccount
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

## ACTION `modifybudget`

Modify draft WPS proposal budget

- Authority:  `proposer`

### params

- `{name} proposer` - proposer of proposal
- `{name} proposal_name` - proposal name
- `{asset} monthly_budget` - monthly budget payment request
- `{uin8_t} duration` - monthly budget duration (maximum of 6 months)

### example

```bash
cleos push action eosio.wps modifybudget '["myaccount", "mywps", "500.0000 EOS", 2]' -p myaccount
```

## ACTION `setproposer`

Set proposer's metadata

- Authority:  `proposer`

### params

- `{name} proposer` - proposer of proposal
- `{map<name, string>} metadata_json` - a sorted container of <key, value>

### example

```bash
cleos push action eosio.wps setproposer '["myaccount", [{"key":"region", value":"global"}]]' -p myaccount
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
- `{asset} monthly_budget` - monthly budget payment request
- `{uint8_t} duration` - monthly budget duration (maximum of 6 months)
- `{asset} total_budget` - total budget payment request
- `{map<name, string>} proposal_json` - a sorted container of <key, value>
- `{time_point_sec} created` - time proposal was created (UTC)
- `{time_point_sec} start` - start of voting period (UTC)
- `{time_point_sec} end` - end of voting period (UTC)

### example

```json
{
  "proposer": "myaccount",
  "proposal_name": "mywps",
  "title": "My WPS",
  "monthly_budget": "500.0000 EOS",
  "duration": 2,
  "total_budget": "1000.0000 EOS",
  "proposal_json": [
    { "key": "category", "value": "other" },
    { "key": "region", "value": "global" }
  ],
  "activated": "2019-11-05T12:10:00",
  "start": "2019-11-01T00:00:00",
  "end": "2019-12-01T00:00:00"
}
```

## TABLE `votes`

- `{name} proposal_name` - The proposal's name, its ID among all proposals
- `{int16_t} total_net_votes` - total net votes
- `{map<name, name>} votes` - a sorted container of <voter, vote>

### example

```json
{
  "proposal_name": "mywps",
  "total_net_votes": 2,
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

## TABLE `deposits`

- `{name} account` - account balance owner
- `{asset} balance` - token balance amount

### example

```json
{
  "account": "myaccount",
  "balance": "100.0000 EOS"
}
```

## TABLE `drafts`

**scope**: `proposer`

- `{name} proposer` - proposer of proposal
- `{name} proposal_name` - proposal name
- `{string} title` - proposal title
- `{asset} monthly_budget` - monthly budget payment request
- `{uint8_t} duration` - monthly budget duration (maximum of 6 months)
- `{asset} total_budget` - total budget payment request
- `{map<name, string>} proposal_json` - a sorted container of <key, value>

### example

```json
{
  "proposer": "myaccount",
  "proposal_name": "mywps",
  "title": "My WPS",
  "monthly_budget": "500.0000 EOS",
  "duration": 2,
  "total_budget": "1000.0000 EOS",
  "proposal_json": [
    { "key": "category", "value": "other" },
    { "key": "region", "value": "global" }
  ]
}
```

## TABLE `proposers`

- `{name} proposer` - proposer of proposal
- `{map<name, string>} metadata_json` - a sorted container of <key, value>

### example

```json
{
  "proposer": "myaccount",
  "metadata_json": [
    { "key": "region", "value": "global" }
  ]
}
```

## TABLE `periods`

- `{time_point_sec} period` - current voting period
- `{set<name>} proposals` - set of proposal names

### example

```json
{
  "period": "2019-11-01T00:00:00",
  "proposals": ["mywps"],
}
```

## TABLE `transfers` (TESTING ONLY)

- `{uint64_t} id` - incoming transfer identifier
- `{name} type` - type of transfer
- `{name} from` - sender of transfer
- `{name} to` - receiver of transfer
- `{asset} quantity` - transfer quantity amount
- `{string} memo` - transfer memo
- `{time_point_sec} timestamp` - timestamp of transfer
- `{checksum256} tx_id` - transaction ID

### example

```json
{
  "id": 0,
  "from": "myaccount",
  "to": "eosio.wps",
  "quantity": "50.0000 EOS",
  "memo": "donation",
  "timestamp": "2019-11-01T00:00:00",
  "tx_id": "<TRANSACTION ID>"
}
```