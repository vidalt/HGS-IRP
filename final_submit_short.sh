#!/bin/bash

for Seed in {0,1,10,50,100,1000,10000}; do
#echo "Processing folder: $folder"  
	./irp $1 -seed "$Seed" -type 38 -veh $2
done