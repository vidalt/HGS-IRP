#!/bin/bash
#SBATCH --cpus-per-task=1
#SBATCH --mem=25G
#SBATCH --time=02:20:00
#SBATCH --partition=optimum
#SBATCH --array=1-60
#SBATCH --output=Output_4/arrayjob_%A_%a.out

FOLDERS=( "/home/zhajin/IRPNSO/Data/Big/Istanze001005")


i=1
for folder in "${FOLDERS[@]}"; do
#    echo "Processing folder: $folder"  
    for file in "$folder"/*_1.dat; do 
        if [ $SLURM_ARRAY_TASK_ID -eq $i ]
    	then
		./irp "$file" -seed $1 -type 38 -veh 2 -stock 1000000
    	fi
    	(( i = $i +1 ))
    done
done