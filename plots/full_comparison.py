# -*- coding: utf-8 -*-
"""
Created on Fri Jan  7 19:14:22 2022

@author: aoust
"""

import pandas
import matplotlib.pyplot as plt
import scipy.stats
import numpy as np

d_3_RH = pandas.read_csv("../output/output_heur3_1.500000.csv",sep = ";",header=2)
d_3_RH["Instance name "] = d_3_RH["Instance name "] + "_3channels"
d_6_RH = pandas.read_csv("../output/output_heur6_1.500000.csv",sep = ";",header=2)
d_6_RH["Instance name "] = d_6_RH["Instance name "] + "_6channels"
d_RH = pandas.concat([d_3_RH,d_6_RH])

d_3_GH = pandas.read_csv("../output/output_greedy3.csv",sep = ";",header=1)
d_3_GH["Instance name "] = d_3_GH["Instance name "] + "_3channels"
d_6_GH = pandas.read_csv("../output/output_greedy6.csv",sep = ";",header=1)
d_6_GH["Instance name "] = d_6_GH["Instance name "] + "_6channels"
d_GH = pandas.concat([d_3_GH,d_6_GH])


d_3_MILP = pandas.read_csv("../output/output_milp3.csv",sep = ";",header=2)
d_3_MILP["Instance name "] = d_3_MILP["Instance name "] + "_3channels"
d_6_MILP = pandas.read_csv("../output/output_milp6.csv",sep = ";",header=2)
d_6_MILP["Instance name "] = d_6_MILP["Instance name "] + "_6channels"
d_MILP = pandas.concat([d_3_MILP,d_6_MILP])

d_total = pandas.DataFrame()
d_total["Instance name "] = d_RH["Instance name "]
d_total["|V|"] = d_RH['|I| ']+d_RH['|J|']
d_total["CPLEX time"] = d_MILP[" CPLEX time"]
d_total["CPLEX UB"] = d_MILP[" CPLEX UB"]
d_total["CPLEX LB"] = d_MILP[" CPLEX LB"]
d_total["GH1 UB"] = d_GH["GH1 UB"]
d_total["GH1 time"] = d_GH["GH1 time"]
d_total["GH2 UB"] = d_GH["GH2 UB"]
d_total["GH2 time"] = d_GH["GH2 time"]
d_total["RH1 UB"] = d_RH["RH1 UB"]
d_total["RH1 time"] = d_RH["RH1 time"]
d_total["RH2 UB"] = d_RH["RH2 UB"]
d_total["RH2 time"] = d_RH["RH2 time"]
f = open('../full_results_table.txt','w')
f.write(d_total.to_markdown())
f.close()

d_total['diff RH2'] = d_total["CPLEX UB"]-d_total["RH2 UB"]
d_total['diff GH2'] = d_total["CPLEX UB"]-d_total["GH2 UB"]


"""Time comparison """
print(scipy.stats.wilcoxon(d_total["RH2 time"],d_total["CPLEX time"]))
print(scipy.stats.wilcoxon(d_total["GH2 time"],d_total["RH2 time"]))
print(len(d_total[d_total["GH2 time"]<=1]),len(d_total[d_total["RH2 time"]<=1]),len(d_total[d_total["CPLEX time"]<=1]))
print(d_total["GH2 time"].mean(),d_total["RH2 time"].mean(),d_total["CPLEX time"].mean())
print(d_total["GH2 time"].std(),d_total["RH2 time"].std(),d_total["CPLEX time"].std())
print("Quantiles GH2 times = {0}".format((d_total["GH2 time"]).quantile([0,0.25,0.5,0.75,0.8,0.85,0.9,1])))
print("Quantiles RH2 times = {0}".format((d_total["RH2 time"]).quantile([0,0.25,0.5,0.75,0.8,0.85,0.9,1])))
print("Quantiles CPLEX times = {0}".format((d_total["CPLEX time"]).quantile([0,0.25,0.5,0.75,0.8,0.85,0.9,1])))

def histogram(serie):
    serie = list(serie)
    serie.sort()
    L = len(serie)
    return serie, list(range(1,1+L))

def time_profile():
    xMilp, yMilp = histogram(d_total["CPLEX time"])
    xGH, yGH = histogram(d_total["GH time"])
    xRH, yRH = histogram(d_total["RH time"])
    L = len(xMilp)
    M = max(max(max(xMilp),max(xGH)),max(xRH))
    plt.plot(xGH+[M],yGH+[L], color = 'grey', label = "GH2",linestyle = "--")
    plt.plot(xRH+[M],yRH+[L], color = 'grey', label = "RH2")
    plt.plot(xMilp+[M],yMilp+[L], color = 'black', label = "CPLEX")

    plt.xscale('log')
    plt.legend()
    plt.xlabel("Time (s)")
    plt.ylabel("Number of instances solved within this time")
    plt.legend()
    plt.savefig("TimeDistributions.png")
    plt.close()
    

def share_instance_good():
    d3_total,d6_total = d_total[:108],d_total[108:]
    N_RH3 = len(d3_total['diff RH2'][d3_total['diff RH2']>=0])
    N_GH3 = len(d3_total['diff GH2'][d3_total['diff GH2']>=0])
    print(N_RH3,N_GH3)
    N_RH6 = len(d6_total['diff RH2'][d6_total['diff RH2']>=0])
    N_GH6 = len(d6_total['diff GH2'][d6_total['diff GH2']>=0])
    print(N_RH6,N_GH6)
    
    N_RH3 = len(d3_total['diff RH2'][d3_total['diff RH2']>0])
    N_GH3 = len(d3_total['diff GH2'][d3_total['diff GH2']>0])
    print(N_RH3,N_GH3)
    N_RH6 = len(d6_total['diff RH2'][d6_total['diff RH2']>0])
    N_GH6 = len(d6_total['diff GH2'][d6_total['diff GH2']>0])
    print(N_RH6,N_GH6)
    
def table_diff_instances():
    df = d_total[["Instance name ","|V|","CPLEX time","GH2 time", "RH2 time","CPLEX LB","CPLEX UB","GH2 UB", "RH2 UB"]]
    df =df[df['CPLEX LB']<df['CPLEX UB']]
    f = open('../results_diff_instances.txt','w')
    f.write(df.to_latex(index=False))
    f.close()
    return df

def comparison_withCPLEX(channels_number):
    serie1 = -100*d_total['diff GH2']/d_total["CPLEX UB"]
    serie2 = -100*d_total['diff RH2']/d_total["CPLEX UB"]
    if channels_number==3:
        serie1,serie2 = serie1[:108],serie2[:108]
    elif channels_number == 6:
        serie1,serie2 = serie1[108:],serie2[108:]
    else:
        assert(False)
            
    print("Nbr of cases with more than 50%, GH2, RH2 = {0},{1}".format(len(serie1[serie1>=50]),len(serie2[serie2>=50])))
    plt.hist([serie1,serie2], bins=40,color=["grey",'black'],label = ["GH2","RH2"])
    plt.xlabel("Obj. value advantage (in %) of CPLEX over heuristics")
    plt.ylabel("Number of instances")
    plt.legend()
    plt.savefig("cplexAdvantage{0}.png".format(channels_number))
    plt.close()

def comparison1v1UB(serie1,serie2,name_title,name_file):
    print("Descriptive statistics")
    mean = np.mean(100*(serie2-serie1)/serie1)
    print("Mean gap = {0}%".format(mean))
    mannwhitneyutest_result = scipy.stats.mannwhitneyu(serie1,serie2)
    wilcoxontest_result = scipy.stats.wilcoxon(serie1,serie2)
    print(mannwhitneyutest_result,wilcoxontest_result)
    serie1_3channel, serie1_6channel = serie1[:108], serie1[108:]
    serie2_3channel, serie2_6channel = serie2[:108], serie2[108:]
    savings_3channel = 100*(serie2_3channel-serie1_3channel)/serie1_3channel
    savings_6channel = 100*(serie2_6channel-serie1_6channel)/serie1_6channel
    plt.hist([savings_3channel,savings_6channel], bins=20,color=["grey",'black'],label = ["3 channels","6 channels"])
    plt.xlabel(name_title)
    plt.ylabel("Number of instances")
    plt.legend()
    plt.savefig(name_file+".png")
    plt.close()
    
def comparison1v1time(serie1,serie2,name_title,name_file):
    print("Descriptive statistics")
    mean = np.mean((serie2-serie1))
    print("Mean gap = {0}s".format(mean))
    mannwhitneyutest_result = scipy.stats.mannwhitneyu(serie2,serie1,alternative='greater')
    wilcoxontest_result = scipy.stats.wilcoxon(serie2,serie1,alternative='greater')
    print(mannwhitneyutest_result,wilcoxontest_result)
    serie1_3channel, serie1_6channel = serie1[:108], serie1[108:]
    serie2_3channel, serie2_6channel = serie2[:108], serie2[108:]
    savings_3channel = (serie2_3channel-serie1_3channel)
    savings_6channel = (serie2_6channel-serie1_6channel)
    plt.hist([savings_3channel,savings_6channel], bins=20,color=["grey",'black'],label = ["3 channels","6 channels"])
    plt.xlabel(name_title)
    plt.ylabel("Number of instances")
    plt.legend()
    plt.savefig(name_file+".png")
    plt.close()

def comparison1v1UBscatter(serie1,serie2,sizes,name_xtitle,name_ytitle,name_file):
    m = max(serie1.max(),serie2.max())
    sizes1, sizes2 = sizes[:108], sizes[108:]
    serie1_3channel, serie1_6channel = serie1[:108], serie1[108:]
    serie2_3channel, serie2_6channel = serie2[:108], serie2[108:]
    plt.plot(np.linspace(10,m,1000),np.linspace(10,m,1000),color = 'grey',linestyle="--")
    plt.scatter(serie1_3channel,serie2_3channel,color="grey",label = "3 channels")
    plt.scatter(serie1_6channel,serie2_6channel,color="black",label = "6 channels")
    #plt.scatter(serie1_3channel,serie2_3channel,s=sizes1,color="grey",label = "3 channels")
    #plt.scatter(serie1_6channel,serie2_6channel,s=sizes2,color="black",label = "6 channels")
    plt.xlabel(name_xtitle)
    plt.ylabel(name_ytitle)
    plt.xscale('log')
    plt.yscale('log')
    plt.legend()
    plt.savefig(name_file+".png")
    plt.close()
    


print('--------------UB greedy vs relax--------------')
comparison1v1UB(d_GH['GH2 UB'],d_RH['RH2 UB'],"Relative objective gap (%) from GH2 to RH2","greedy_to_relax_cost_comparison")
comparison1v1UB(d_RH['RH2 UB'],d_GH['GH2 UB'],"Relative objective gap (%) from RH2 to GH2","relax_to_greedy_cost_comparison")
comparison1v1UB(d_MILP[' CPLEX UB'],d_GH['GH2 UB'],"Relative objective gap (%) from CPLEX to GH2","CPLEX_to_greedy_cost_comparison")
comparison1v1UB(d_MILP[' CPLEX UB'],d_RH['RH2 UB'],"Relative objective gap (%) from CPLEX to RH2","CPLEX_to_Rh_cost_comparison")
#comparison1v1UBscatter(d_GH['GH2 UB'].values,d_RH['RH2 UB'].values,(d_GH['|I| ']+d_GH['|J|']).values.astype('float'),"Objective cost with GH2","Objective cost with RH2","greedy_to_relax_cost_scatter")
#comparison1v1time(d_GH['GH2 time'],d_RH['RH2 time'],"Absolute time difference (s) from GH2 to RH2","greedy_to_relax_time_comparison")