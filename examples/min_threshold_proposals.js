// 1. iterate over all active proposals using bystatus index
// 2. filter out any proposal that does not meet vote_margin threshold
// 3. order proposals by total_net_votes (highest/lowest)
// 4. stop paying proposals once max_monthly_budget is hit
// 5. set proposals that got paid to completed / proposals that did not met threshold to expired and proposals that have more periods remain as active for the next voting period
