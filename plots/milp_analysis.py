import pandas
import matplotlib.pyplot as plt
import numpy as np

D3=pandas.read_csv("../output/output_milp3.csv",sep = ";",header=2)
D6=pandas.read_csv("../output/output_milp6.csv",sep = ";",header=2)
D6 = D6[D6[' CPLEX UB']!=-1]

"MILP_cardinality_vs_time.png"
plt.scatter(D3["|I| "]+D3["|J|"],D3[" CPLEX time"],marker="x",label="3 channels",color = 'grey')
plt.scatter(D6["|I| "]+D6["|J|"],D6[" CPLEX time"],marker="x",label="6 channels",color = 'black')
plt.xlabel("Number of nodes (|V|)")
plt.ylabel("CPLEX Time (s)")
plt.yscale("log")
plt.legend()
plt.savefig("MILP_cardinality_vs_time.png")
plt.close()

"MILP_time_distribution.png"
hx3, hy3, _ = plt.hist(np.log(D3[" CPLEX time"]), bins=300,color="grey")
cumsum3 = np.cumsum(hx3)
hx6, hy6, _ = plt.hist(np.log(D6[" CPLEX time"]), bins=300,color="grey")
cumsum6 = np.cumsum(hx6)
plt.close()
plt.plot(np.exp(hy3[1:]),cumsum3, color = 'grey', label = "3 channels")
plt.plot(np.exp(hy6[1:]),cumsum6, color = 'black', label = "6 channels")
plt.xscale('log')
plt.legend()
plt.xlabel("CPLEX Time (s)")
plt.ylabel("Number of instances solved within this time")
plt.savefig("MILP_time_distribution.png")
plt.close()

"MILP_time3_vs_time6.png"
plt.plot(np.linspace(0.001,3600,1000),np.linspace(0.001,3600,1000),color = 'grey',linestyle="--")
plt.scatter(D3[" CPLEX time"],D6[" CPLEX time"],marker="x",color = 'black')
plt.xlabel("CPLEX time (s) for 3 channels")
plt.ylabel("CPLEX time (s) for 6 channels")
plt.xscale("log")
plt.yscale("log")
plt.savefig("MILP_time3_vs_time6.png")
plt.close()

"MILP_savings_3_to_6.png"
savings = 100*(D3[' CPLEX UB']-D6[' CPLEX UB'])/D3[' CPLEX UB']
hx3, hy3, _ = plt.hist((savings), bins=10,color="grey")
plt.xlabel("Relative saving (3->6 channels)")
plt.ylabel("Number of instances")
plt.savefig("MILP_savings_3_to_6.png")
plt.close()

"MILP_optimality_gap_scatter.png"
partial3,partial6 = D3[D3[" CPLEX time"]>3600], D6[D6[" CPLEX time"]>3600]
totally_solved3, totally_solved6 = 108 - len(partial3),108 - len(partial6)
gap3, gap6 = 100*(partial3[' CPLEX UB'] - partial3[' CPLEX LB'])/(partial3[' CPLEX UB']), 100*(partial6[' CPLEX UB'] - partial6[' CPLEX LB'])/(partial6[' CPLEX UB'])
plt.scatter(partial3["|I| "]+partial3["|J|"],gap3,marker="x",label="3 channels",color = 'grey')
plt.scatter(partial6["|I| "]+partial6["|J|"],gap6,marker="x",label="6 channels",color = 'black')
plt.xlabel("Number of nodes (|V|)")
plt.ylabel("Relative optimality gap (%)")
plt.savefig("MILP_optimality_gap_scatter.png")
plt.legend()
plt.close()

"MILP_gap_distribution.png"
hx3, hy3, _ = plt.hist((gap3), bins=300,color="grey")
cumsum3 = np.cumsum(hx3)
hx6, hy6, _ = plt.hist(gap6, bins=300,color="grey")
cumsum6 = np.cumsum(hx6)
plt.close()
hy3[-1],hy6[-1]=100,100
plt.plot((hy3),list(totally_solved3+cumsum3)+[108], color = 'grey', label = "3 channels")
plt.plot((hy6),list(totally_solved6+cumsum6)+[108], color = 'black', label = "6 channels")
plt.legend()
plt.xlabel("Relative optimality gap (%)")
plt.ylabel("Number of instances with smaller optimality gap")
plt.savefig("MILP_gap_distribution.png")
plt.close()

