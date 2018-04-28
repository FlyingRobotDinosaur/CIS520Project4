#!/bin/bash

#SBATCH --mem=16G
#SBATCH --time=10:00:00
#SBATCH --constraint=dwarves
#SBATCH -J Project4_OMP1
#SBATCH --cpus-per-task=1


for i in 100 1000 5000 10000 50000 100000 250000
	do
		$HOME/Project4/LCSomp 1 $i
	done | grep -w DATA > $HOME/Project4/OMP1_1.csv
