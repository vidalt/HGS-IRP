#!/bin/bash

for Seed in {0,8,20,30,40,50,100,200,500,1000}; do
	sbatch final_slurm-batch-script-long.sh "$Seed" 
done