import { Action } from "eosjs/dist/eosjs-serialize"

export function createAccount( name: string, key = "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV" ): Action {
  const authority = {
    threshold: 1,
    keys: [{
        key,
        weight: 1
      }
    ]
  }
  return {
    account: "eosio",
    name: "newaccount",
    authorization: [{ actor: "eosio", permission: "active"}],
    data: {
      creator: "eosio",
      name,
      owner: authority,
      active: authority
    }
  }
}

export function transfer( data: { from: string, to: string, quantity: string, memo: string } ): Action {
  return {
    account: "eosio.token",
    name: "transfer",
    authorization: [{ actor: data.from, permission: "active"}],
    data
  }
}

export function propose( data: { proposer: string, proposal_name: string, title: string, proposal_json: string, budget: string, payments: number }): Action {
  return {
    account: "eosio.wps",
    name: "propose",
    authorization: [{ actor: data.proposer, permission: "active"}],
    data
  }
}

export function vote( data: { voter: string, proposal_name: string, vote: string } ): Action {
  return {
    account: "eosio.wps",
    name: "propose",
    authorization: [{ actor: data.voter, permission: "active"}],
    data
  }
}

export function init( data: { current_voting_period: string } ): Action {
  return {
    account: "eosio.wps",
    name: "propose",
    authorization: [{ actor: "eosio.wps", permission: "active"}],
    data
  }
}

export function active( data: { proposer: string, proposal_name: string } ): Action {
  return {
    account: "eosio.wps",
    name: "propose",
    authorization: [{ actor: "eosio.wps", permission: "active"}],
    data
  }
}

export function settings( data: { vote_margin: number, deposit: string, interval: number }): Action {
  return {
    account: "eosio.wps",
    name: "propose",
    authorization: [{ actor: "eosio.wps", permission: "active"}],
    data
  }
}
