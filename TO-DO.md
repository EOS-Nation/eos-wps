## To-Dos

- add `refresh()` action to re-calculate votes from a particular BP
- if proposal doesn't met threshold, cancel all subsequent proposals
- add comment on `vote`
    - add `comments` TABLE
    - add `comment` OPTIONAL param in `vote`

## Deploy Workflow

MSIG #1
1. create `eosio.wps` account
2. deploy smart contract

MSIG #2
3. `init` contract parameters
4. send EOS funds to contract
5. `start` voting period
