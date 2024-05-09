#!/bin/bash
#SBATCH --cpus-per-task=1
#SBATCH --mem=25G
#SBATCH --time=01:30:00
#SBATCH --partition=optimum
#SBATCH --array=1-240
#SBATCH --output=arrayjob_%A_%a.out

FOLDERS=("/home/zhajin/IRP-nov28/Data/Big/Istanze0105" "/home/zhajin/IRP-nov28/Data/Big/Istanze001005")


i=1
for folder in "${FOLDERS[@]}"; do
#    echo "Processing folder: $folder"  
    for file in "$folder"/*.dat; do
        if [ $SLURM_ARRAY_TASK_ID -eq $i ]
    	then
	    ./irp "$file" -seed 1000 -type 38 -veh 8 -stock 100
    	fi
    	(( i = $i +1 ))
    done
done

sleep 60