#!/bin/bash

program="./irp-5-ds-single-ri-ds-ri"

seeds=(0 8 20 30 40 50 100 200 500 1000)

root_instance_folder="./Data/Test"
instance_subfolders=("0105h6")

solutionFolder="./Data/Test/Solutions.5.1"

# Iterate through each folder
for sub_folder in "${instance_subfolders[@]}"; do
  # Iterate through each instance in the folder
  instance_folder="$root_instance_folder/$sub_folder"
  for instance in "$instance_folder"/*.dat; do
    # Construct the path to the solution file
    # solution="${instance%.txt}.sol"
    solution="$solutionFolder/$sub_folder/$(basename "$instance" .dat).sol"
    bks_file="$solutionFolder/$sub_folder/$(basename "$instance" .dat).bks" 

    nbVehicles=$(basename "$instance" .dat| awk -F'_' '{print $2}')
    echo "nbVehicle: $nbVehicles"
    
    # Run the program for each seed
    for seed in "${seeds[@]}"; do
      echo "Running: $program $instance -sol $solution.$seed -bks $bks_file -seed $seed -type 38 -veh $nbVehicles -stock 1000000"
      $program $instance -sol $solution.$seed -bks $bks_file -seed $seed -type 38 -veh $nbVehicles -stock 1000000
    done
  done
done