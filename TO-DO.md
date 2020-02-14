- if proposal doesn't met threshold, cancel all subsequent proposals
- 24 hour BP threshold & vote weight of min 100 EOS
- `producers` TABLE
- add comment on `vote`
- check `balance`
- add `start` ACTION to start
- add `pause` ACTION to stop all actions except for `withdraw`

## Deploy Workflow

1. create `eosio.wsp` account
2. deploy smart contract
3. `init` contract parameters
4. send EOS funds to contract
5. `start` voting period
