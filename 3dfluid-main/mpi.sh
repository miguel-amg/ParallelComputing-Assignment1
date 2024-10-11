#!/bin/bash
#SBATCH --partition=cpar

perf record ./fluid_sim
perf report --stdio>result
