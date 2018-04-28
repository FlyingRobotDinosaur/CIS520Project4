#!/bin/bash
#SBATCH --mem=2G
#SBATCH --time=1:00:00
#SBATCH --constraint=dwarves
#SBATCH -J MPI_TEST
##module load OpenMPI
mpirun -np 2 MPI 100 | grep -w DATA > $HOME/Project4/MPI.csv
