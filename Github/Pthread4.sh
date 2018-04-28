#!/bin/bash

#SBATCH --mem=16G
#SBATCH --time=6:30:00
#SBATCH --constraint=dwarves
#SBATCH -J Project4_Pthread4
#SBATCH --cpus-per-task=4

for i in 100 1000 5000 10000 50000 100000 250000
	do
		$HOME/Project4/Pthread 4 $i
	done | grep -w DATA > $HOME/Project4/Pthread4_1.csv
