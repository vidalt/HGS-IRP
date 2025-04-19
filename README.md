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
