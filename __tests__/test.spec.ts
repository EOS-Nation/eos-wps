import { transact } from "./utils";
import { propose, transfer } from './actions';
import { get_proposals } from './tables';

test("propose & deposit", async () => {
  // propose
  const proposal_name = "mywps";
  await transact([ propose({
    proposer: "myaccount",
    proposal_name,
    title: "My WPS",
    proposal_json: JSON.stringify({category: "other", region: "global"}),
    budget: "500.0000 EOS",
    payments: 1,
  }) ]);

  // deposit
  const quantity = "100.0000 EOS";
  await transact([ transfer({
    from: "myaccount",
    to: "eosio.wps",
    quantity,
    memo: "mywps"
  }) ]);

  // test table
  const proposals = await get_proposals( proposal_name );
  expect( proposals.proposal_name ).toBe(proposal_name);
  expect( proposals.deposit ).toBe(quantity);
});

