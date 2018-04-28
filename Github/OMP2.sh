#!/bin/bash

#SBATCH --mem=16G
#SBATCH --time=7:30:00
#SBATCH --constraint=dwarves
#SBATCH -J Project4_OMP2
#SBATCH --cpus-per-task=2


for i in 100 1000 5000 10000 50000 100000 250000
	do
		$HOME/Project4/LCSomp 2 $i
	done | grep -w DATA > $HOME/Project4/OMP2_1.csv
