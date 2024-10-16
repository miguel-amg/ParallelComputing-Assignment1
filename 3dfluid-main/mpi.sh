#!/bin/bash
#SBATCH --partition=cpar

perf record -g ./fluid_sim
perf report --stdio>result
