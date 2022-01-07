import pandas
import matplotlib.pyplot as plt
import numpy as np
n_bins = 150
D=pandas.read_csv("heur_final.csv",sep = ";")
print(D)


fig, ax = plt.subplots(figsize=(8, 4))


x1 =(D["Val Greedy-Tabu10Par"]-D["Val MILP"])/D["Val MILP"]
n, bins, patches = ax.hist(x1, n_bins, histtype='step',
                           cumulative=True, label='Greedy-Tabu10')


x2 =(D["Val HrelaxReg"]-D["Val MILP"])/D["Val MILP"]
n, bins, patches = ax.hist(x2, n_bins, histtype='step',
                           cumulative=True, label='Hrelax2')

x4 =(D["Val CG"]-D["Val MILP"])/D["Val MILP"]
n, bins, patches = ax.hist(x4, n_bins, histtype='step',
                           cumulative=True, label='H CG')

x3 =(D["Val Hrelax"]-D["Val MILP"])/D["Val MILP"]
n, bins, patches = ax.hist(x3, n_bins, histtype='step',
                           cumulative=True, label='Hrelax')



plt.legend(loc='lower right')
plt.xlim(-2, 8)
plt.xlabel("Relative gap to CPLEX solution (in %)")
plt.ylabel("Nb of instances below threshold")


#plt.xscale("log")
plt.savefig("heur_val.png")


fig, ax = plt.subplots(figsize=(8, 4))


x1 =(D["Time Greedy-Tabu10Par "]-D["Time MILP"])/D["Time MILP"]
n, bins, patches = ax.hist(x1, n_bins, histtype='step',
                           cumulative=True, label='Greedy-Tabu10')

x3 =(D["Time Hrelax"]-D["Time MILP"])/D["Time MILP"]
n, bins, patches = ax.hist(x3, n_bins, histtype='step',
                           cumulative=True, label='Hrelax')

x2 =(D["Time HrelaxReg"]-D["Time MILP"])/D["Time MILP"]
n, bins, patches = ax.hist(x2, n_bins, histtype='step',
                           cumulative=True, label='Hrelax 2')

x4 =(D["Time CG"]-D["Time MILP"])/D["Time MILP"]
n, bins, patches = ax.hist(x4, n_bins, histtype='step',
                           cumulative=True, label='H CG')


plt.legend(loc='lower right')
plt.xlim(-2, 8)
plt.xlabel("Relative time compared to CPLEX (in %)")
plt.ylabel("Nb of instances below threshold")


#plt.xscale("log")
plt.savefig("heur_time.png")




