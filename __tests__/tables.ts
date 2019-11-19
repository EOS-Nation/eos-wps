import { rpc } from './config';

interface Proposals {
  proposer: string;
  proposal_name: string;
  title: string;
  proposal_json: string;
  budget: string;
  payments: number;
  deposit: string;
  status: string;
  start: string;
  end: string;
}

export async function get_proposals(proposal_name: string): Promise<Proposals> {
  const table = await rpc.get_table_rows({
    json: true,
    code: "eosio.wps",
    scope: "eosio.wps",
    table: "proposals",
    lower_bound: proposal_name,
    upper_bound: proposal_name
  })
  return table.rows[0]
}