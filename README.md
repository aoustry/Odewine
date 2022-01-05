# Odewine
Optimal Deployment of Wireless Networks

GitHub repository accompanying the paper A. Oustry, M. Le Tilly, T. Clausen, C. D'Ambrosio, L. Liberti, "Optimal deployment of indoor wireless local area networks", Submitted.

## Requirements and installation
The following softwares are required to execute the code
- Python3
- G++ compiler
- The CPLEX API for C++

To generate one of the 3 executables, modify the makefile to compile:
- main_milp.cpp for the MILP algorithm,
- main_greedy.cpp for the greedy heuristics,
- main_heur_inf.cpp for the relaxation-based heuristics.

## Numerical experiments

To run the numerical experiments that are presented in the aforementioned article, execute the following commands with frequency_number = 3 or 6:

```
./main_milp list_full.txt [frequency_number]
```
and
```
./main_greedy test.txt [frequency_number] 
```
and
```
./main_heur_inf test.txt [frequency_number] 20
```


------------------------------------------------------------------------------------------

Researchers affiliated with

(o) LIX CNRS, École polytechnique, Institut Polytechnique de Paris, 91128, Palaiseau, France

(o) École des Ponts, 77455 Marne-La-Vallée

Developed as part of the ODEWINE project sponsored by the Cisco Research Foundation

