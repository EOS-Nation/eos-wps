<h1 class="contract">submitdraft</h1>

---
spec_version: "0.2.0"
title: Submit Draft
summary: 'Submit a WPS draft proposal'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

{{proposer}} agrees to submit the draft proposal '{{title}}' with the proposal identifier of {{proposal_name}}. This draft includes a budget of {{monthly_budget}} for {{duration}}.

RAM will deducted from {{proposer}}’s resources to create the necessary records.

<h1 class="contract">vote</h1>

---
spec_version: "0.2.0"
title: Proposal Vote
summary: 'Vote for a WPS proposal'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

{{voter}} agrees to vote '{{vote}}' on the proposal '{{proposal_name}}'.

RAM will deducted from {{proposer}}’s resources to create the necessary records.

<h1 class="contract">init</h1>

---
spec_version: "0.2.0"
title: Init
summary: 'Initialize the WPS contract'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

This action will initialize the WPS and its initial voting period.

<h1 class="contract">start</h1>

---
spec_version: "0.2.0"
title: Start
summary: 'Start WPS voting period'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

This action will start the voting period.

<h1 class="contract">setparams</h1>

---
spec_version: "0.2.0"
title: Set Parameters
summary: 'Set paramaters for WPS contract'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

Set the parameters of the WPS contract to the values provided.

<h1 class="contract">activate</h1>

---
spec_version: "0.2.0"
title: Activate Proposal
summary: 'Activate WPS proposal in the specified voting period'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

{{proposer}} agrees to activate the draft proposal '{{proposal_name}}' within the voting period of '{{start_voting_period}}'.

This action will make the minimum deposit from {{proposer}} non-refundable and the proposal can no longer be modified.

<h1 class="contract">refund</h1>

---
spec_version: "0.2.0"
title: Refund
summary: 'Refund proposal deposit'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

{{account}} agrees to refund any available funds from the WPS contract.

<h1 class="contract">canceldraft</h1>

---
spec_version: "0.2.0"
title: Cancel Draft
summary: 'Cancel a WPS draft proposal'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

{{proposer}} agrees to cancel the draft with the proposal identifier of {{proposal_name}}.

This action will release any RAM utilized by this proposal.

<h1 class="contract">modifydraft</h1>

---
spec_version: "0.2.0"
title: Modify Draft
summary: 'Modify draft WPS proposal'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

{{proposer}} agrees to modify the draft proposal for '{{title}}' with the proposal identifier of {{proposal_name}}.

RAM will deducted from {{proposer}}’s resources to create the necessary records.

<h1 class="contract">modifybudget</h1>

---
spec_version: "0.2.0"
title: Modify Budget
summary: 'Modify draft WPS proposal budget'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

{{proposer}} agrees to modify the budget of the draft proposal with the identifier of '{{proposal_name}}' to contain a budget of {{monthly_budget}} and a duration of {{duration}} months.

RAM will deducted from {{proposer}}’s resources to create the necessary records.

<h1 class="contract">setproposer</h1>

---
spec_version: "0.2.0"
title: Set Proposer
summary: 'Set proposer's metadata'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

{{proposer}} agrees to modify the metadata for their account with the parameters specified.

RAM will deducted from {{proposer}}’s resources to create the necessary records.

<h1 class="contract">complete</h1>

---
spec_version: "0.2.0"
title: Complete
summary: 'Complete WPS voting period'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

This action will complete the current voting period.

<h1 class="contract">claim</h1>

---
spec_version: "0.2.0"
title: Submit Draft
summary: 'Claim remaining proposal amount, transfer amount to proposer'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

This action will claim any remaining proposal amounts for the {{proposal_name}} and transfer it to the proposer's account.

<h1 class="contract">pause</h1>

---
spec_version: "0.2.0"
title: Pause
summary: 'Pause WPS contract'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

This action will pause the WPS contract.

<h1 class="contract">refresh</h1>

---
spec_version: "0.2.0"
title: Refresh
summary: 'Refresh a WPS voter account'
icon: @ICON_BASE_URL@/@VOTING_ICON_URI@
---

This action will refresh the votes of a specified voter account.

