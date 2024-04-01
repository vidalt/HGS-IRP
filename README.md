# Code for paper on Inventory Routing Problem

## Setup CPlex
- Updating...

## Build
```
make
```
## How to run
```
./irp path-to-instance -seed 1000 -type 38 -veh <number-of-vehicle>
```

- Example: `./irp Data/Small/Istanze0105h3/abs1n5_1.dat -seed 1000 -type 38 -veh 2`

## stockout test4
./irp Data/Small/Istanze0105h3/abs1n10_1.dat -seed 1000 -type 38 -veh 2 -stock 100