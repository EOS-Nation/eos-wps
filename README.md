# EOSIO WPS

## ACTION

- [`propose`](#action-propose)
- [`vote`](#action-vote)
- [`unvote`](#action-unvote)
- [`cancel`](#action-cancel)

## TABLE

- [`draft`](#table-draft)
- [`proposal`](#table-proposal)

## ACTION `propose`

Submit a WPS proposal

- Authority:  `proposer`

### params

- `{name} proposer` - The actual proposer's account
- `{name} proposal_name` - The proposal's name, its ID among all proposals
- `{string} title` - The proposal's title (must be less than 1024 characters)
- `{string} proposal_json` - The proposal's JSON metadata, no specification yet, see Proposal JSON Structure
- `{asset} budget` - payment requested (EOS) per 1 month payment duration
- `{uin8_t} payments` - number of payment duration (maximum of 6 months)

### example

```bash
cleos push action eosio.wps '["myaccount", "mywps", "My WPS", "{\"category\": \"other\", \"region\": \"global\"}", "500.0000 EOS", 1]' -p myaccount
```

## TABLE `proposal`

- `{name} proposer` - The actual proposer's account
- `{name} proposal_name` - The proposal's name, its ID among all proposals
- `{string} title` - The proposal's title (must be less than 1024 characters)
- `{string} proposal_json` - The proposal's JSON metadata, no specification yet, see Proposal JSON Structure
- `{asset} budget` - payment requested (EOS) per 1 month payment duration
- `{uin8_t} payments` - number of payment duration (maximum of 6 months)
- `{time_point_sec} created_at` - The date at which the proposal's was created, ISO 8601 string format (in UTC) without a timezone modifier.

### example

```json
{
  "proposer": "myaccount",
  "proposal_name": "mywps",
  "title": "My WPS",
  "proposal_json": "{\"category\": \"other\", \"region\": \"global\"}",
  "budget": "500.0000 EOS",
  "payments": 1,
  "created_at": "2019-11-03T16:48:21"
}
```
