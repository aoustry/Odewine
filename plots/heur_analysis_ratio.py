import pandas
import matplotlib.pyplot as plt
import numpy as np
n_bins = 600000
D=pandas.read_csv("heur_final.csv",sep = ";")
print(D)


fig, ax = plt.subplots(figsize=(8, 6))


x1 =(D["Val Greedy-Tabu10Par"])/D["Val MILP"]
n, bins, patches = ax.hist(x1, n_bins, histtype='step',
                           cumulative=True, label='Greedy-Tabu10')
x2 =(D["Val HrelaxReg"])/D["Val MILP"]
n, bins, patches = ax.hist(x2, n_bins, histtype='step',
                           cumulative=True, label='Hrelax2')

x3 =(D["Val Hrelax"])/D["Val MILP"]
n, bins, patches = ax.hist(x3, n_bins, histtype='step',
                           cumulative=True, label='Hrelax')

x4 =(D["Val CG"])/D["Val MILP"]
n, bins, patches = ax.hist(x4, n_bins, histtype='step',
                           cumulative=True, label='H CG')




plt.legend(loc='lower right')
plt.xlim(0.1, 20)
plt.xlabel("Ratio to CPLEX solution")
plt.ylabel("Nb of instances below threshold")
plt.xscale("log")
plt.savefig("heur_val_ratio.png")


fig, ax = plt.subplots(figsize=(8, 6))


x1 =(D["Time Greedy-Tabu10Par "])/D["Time MILP"]
n, bins, patches = ax.hist(x1, n_bins, histtype='step',
                           cumulative=True, label='Greedy-Tabu10')

x2 =(D["Time HrelaxReg"])/D["Time MILP"]
n, bins, patches = ax.hist(x2, n_bins, histtype='step',
                           cumulative=True, label='Hrelax2')


x3 =(D["Time Hrelax"])/D["Time MILP"]
n, bins, patches = ax.hist(x3, n_bins, histtype='step',
                           cumulative=True, label='Hrelax')


x4 =(D["Time CG"])/D["Time MILP"]
n, bins, patches = ax.hist(x4, n_bins, histtype='step',
                           cumulative=True, label='H CG')

plt.legend(loc='lower right')
plt.xlim(0.0001, 20)
plt.xlabel("Time ratio compared to CPLEX ")
plt.ylabel("Nb of instances below threshold")
plt.xscale("log")
plt.savefig("heur_time_ratio.png")

plt.figure()
plt.scatter(D["Val MILP"],D["MinValH"],marker = ".")
t = np.linspace(80,10000,1000)
plt.plot(t,t,color = 'black')
plt.xlabel("Best solution found by CPLEX")
plt.ylabel("Best solution found heuristically")
plt.xscale("log")
plt.yscale("log")
plt.savefig("H_MILP_value.png")




