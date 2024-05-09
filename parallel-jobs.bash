#!/bin/bash


FOLDERS=("/home/zhajin/PROJECTS/IRP-SAA-main/Data/Small/Istanze001005h3"
            "/home/zhajin/PROJECTS/IRP-SAA-main/Data/Small/Istanze001005h6"
            "/home/zhajin/PROJECTS/IRP-SAA-main/Data/Small/Istanze0105h3"
            "/home/zhajin/PROJECTS/IRP-SAA-main/Data/Small/Istanze0105h6")

for folder in "${FOLDERS[@]}"; do
#    echo "Processing folder: $folder"  
    for file in "$folder"/*.dat; do
#        ./irp "$file" -seed 1000 -type 38 -veh 2 -stock 100
        echo ./irp "$file" -seed 1000 -type 38 -veh 2 -stock 100
    done
done

FOLDERS=("/home/zhajin/PROJECTS/IRP-SAA-main/Data/Big/Istanze0105" "/home/zhajin/PROJECTS/IRP-SAA-main/Data/Big/Istanze001005")

for folder in "${FOLDERS[@]}"; do
#    echo "Processing folder: $folder"  
    for file in "$folder"/*.dat; do
#        ./irp "$file" -seed 1000 -type 38 -veh 8 -stock 100
        echo ./irp "$file" -seed 1000 -type 38 -veh 8 -stock 100
    done
done
