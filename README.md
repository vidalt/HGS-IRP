

# HGS-IRP: A modern implementation of the Hybrid Genetic Search for the Inventory Routing Problem

This is a modern implementation of the Large Neighborhood and Hybrid Genetic Search for
Inventory Routing Problems.

## References

When using this algorithm in derived academic studies, please refer to the following works:
[1] Vidal, T., Crainic, T. G., Gendreau, M., Lahrichi, N., Rei, W. (2012). 
A hybrid genetic algorithm for multidepot and periodic vehicle routing problems. Operations Research, 60(3), 611-624. 
https://doi.org/10.1287/opre.1120.1048 (Available [HERE](https://w1.cirrelt.ca/~vidalt/papers/HGS-CIRRELT-2011.pdf) in technical report form).

[2] Vidal, T. (2022). Hybrid genetic search for the CVRP: Open-source implementation and SWAP* neighborhood. Computers & Operations Research, 140, 105643.
https://doi.org/10.1016/j.cor.2021.105643 (Available [HERE](https://arxiv.org/abs/2012.10384) in technical report form).

# Code for paper on Inventory Routing Problem


## Build
```
make
```
## How to run
```
./irp path-to-instance -seed <random-seed> -type 38 -veh <number-of-vehicle> -stock <stockout-penalty-parameter> 
```

- Example:
```
./irp Data/Small/Istanze0105h3/abs1n10_1.dat -seed 1000 -type 38 -veh 2 -stock 100
```



## Code structure

The main classes containing the logic of the algorithm are the following:
* **Mutation11 in Local Search.cpp**: The LNS DS operator for IRP.

1. Preprocessing Insertion Costs
   For each past day, it precomputes the insertion cost of the client into all feasible routes using `computeCoutInsertion`. This ensures the optimization model knows where and how much it costs to insert the client in different positions.

2. Evaluate Current Cost
   It calculates the current cost of the client's delivery plan—either with or without stockout penalties—using `evaluateCurrentCost` or `evaluateCurrentCost_stockout`.

3. Build and Solve Lot-Sizing Subproblem
   It prepares insertion options for each day and solves a mixed delivery-inventory problem via `LotSizingSolver`, finding the optimal delivery schedule and quantities for that client.

4. If Not Improving, Exit
   If the model’s proposed cost (objective value) is not significantly better than the current cost, it aborts and returns `0`.

5. Apply and Evaluate Change
   Otherwise, it removes all existing deliveries for the client, applies the new optimized plan (routes and quantities), then checks if the actual cost improvement matches the model's prediction. If yes, it returns `1`, signaling a successful improvement.
* **LotSizingSolver**: After removing a retailer 
𝑖
i from all delivery routes, the DS operator seeks the best reinsertion strategy—optimizing over days, routes, and delivery quantities. Evaluating reinsertion costs in the Inventory Routing Problem (IRP) is particularly challenging, as they are influenced by the retailer’s inventory levels, which themselves depend on the quantities delivered on previous days. The following dynamic programming (DP) algorithm iteratively computes an optimal replenishment plan. Notably, for each day, the cost functions involved are piecewise linear, allowing the algorithm to operate directly at the level of piecewise linear functions.
* **PLFunction**: models a piecewise linear cost function for delivery quantities, enabling efficient dynamic programming over time-dependent inventory and routing decisions by composing, minimizing, and evaluating linear cost segments.

* **LinearPiece**: represents a cost segment between two inventory levels, enabling fine-grained modeling and manipulation of piecewise linear cost functions in dynamic programming for inventory routing.

The details for other files   and supporting components for HGS can be referenced in the https://github.com/vidalt/HGS-CVRP.
