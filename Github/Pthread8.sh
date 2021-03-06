#!/bin/bash

#SBATCH --mem=16G
#SBATCH --time=5:30:00
#SBATCH --constraint=dwarves
#SBATCH -J Project4_Pthread8
#SBATCH --cpus-per-task=8

for i in 100 1000 5000 10000 50000 100000 250000
	do
		$HOME/Project4/Pthread 8 $i
	done | grep -w DATA > $HOME/Project4/Pthread8_1.csv
