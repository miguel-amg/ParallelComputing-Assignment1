# Parallel Computing 

**Authors:** Miguel Carvalho [(Github)](https://github.com/MiguelJacintoML), Miguel Guimarães [(Github)](https://github.com/miguel-amg), Pedro Carneiro [(Github)](https://github.com/PedroCarneiro03).

**University of Minho** - Masters's degree in Software Engineering

**Year 2025**

***

**Assignment #1**
The objective is to analyse and optimise one the fluid dynamics simulation code, it is a 3D version of the Jos Stam's stable fluid solver.
On this assignment phase we will perform a case study analysis and improve the execution time of a non-interactive version of the simulation code.

**Followed steps:** 
- Analyse the code (using the data given in the events.txt file in the repository) and get its profile, to identify 
the key blocks requiring modifications, which will lead to improved execution performance; we suggest the 
use of a call-graph with gprof (as in the lecture slides, see below); 
- Explore the main goal of this work assignment: to reduce the execution time; note that the simulation 
output must be maintained; 
- Estimate the execution performance improvement before applying any modification, since there are a wide 
range of possible optimisation techniques, many of them without any impact on performance; 
- Keep code legibility as much as possible, also a key goal in this work, since it is very relevant to have a clean 
code for upcoming assignment phases.