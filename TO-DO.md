- add `refresh()` action to re-calculate votes from a particular BP
- start() initiates at 00:00 UTC
- ability to complete() and start() could not be executed due to lack of funds
- deposit goes towards funding
- if proposal doesn't met threshold, cancel all subsequent proposals
- 24 hour BP threshold & vote weight of min 100 EOS
- `producers` TABLE
- add comment on `vote`
- check `balance`
- add `start` ACTION to start
- add `pause` ACTION to stop all actions except for `withdraw`

- vote errors:
  - not claimed for the last 24 hours
  - not enough 100 EOS votes
  - nothing changed
- move `proposal_json` to end of TABLE
- add `comments` TABLE
- add `comment` OPTIONAL param in `vote`

## Deploy Workflow

MSIG #1
1. create `eosio.wsp` account
2. deploy smart contract

MSIG #2
3. `init` contract parameters
4. send EOS funds to contract
5. `start` voting period
