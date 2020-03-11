## To-Dos

- if proposal doesn't met threshold, cancel all subsequent proposals
- add comment on `vote`
    - add `comments` TABLE
    - add `comment` OPTIONAL param in `vote`

## Deploy Workflow

**MSIG #1**

- Action 1: create `eosio.wps` account
- Action 2: deploy smart contract

**MSIG #2**

- Action 1: `init` contract parameters
- Action 2: send EOS funds to contract
- Action 3: `start` voting period
