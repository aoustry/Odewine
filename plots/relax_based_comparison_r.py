# -*- coding: utf-8 -*-
"""
Created on Fri Jan  7 10:38:13 2022

@author: aoust
"""
import numpy as np
import pandas
import matplotlib.pyplot as plt
import scipy.stats


d_3_12 = pandas.read_csv("../output/output_heur3_1.200000.csv",sep = ";",header=2)
d_3_12["Instance name "] = d_3_12["Instance name "] + "_3channels"
d_6_12 = pandas.read_csv("../output/output_heur6_1.200000.csv",sep = ";",header=2)
d_6_12["Instance name "] = d_6_12["Instance name "] + "_6channels"
d12 = pandas.concat([d_3_12,d_6_12])

d_3_15 = pandas.read_csv("../output/output_heur3_1.500000.csv",sep = ";",header=2)
d_3_15["Instance name "] = d_3_15["Instance name "] + "_3channels"
d_6_15 = pandas.read_csv("../output/output_heur6_1.500000.csv",sep = ";",header=2)
d_6_15["Instance name "] = d_6_15["Instance name "] + "_6channels"
d15 = pandas.concat([d_3_15,d_6_15])

d_3_2 = pandas.read_csv("../output/output_heur3_2.000000.csv",sep = ";",header=2)
d_3_2["Instance name "] = d_3_2["Instance name "] + "_3channels"
d_6_2 = pandas.read_csv("../output/output_heur6_2.000000.csv",sep = ";",header=2)
d_6_2["Instance name "] = d_6_2["Instance name "] + "_6channels"
d2 = pandas.concat([d_3_2,d_6_2])

d_3_3 = pandas.read_csv("../output/output_heur3_3.000000.csv",sep = ";",header=2)
d_3_3["Instance name "] = d_3_3["Instance name "] + "_3channels"
d_6_3 = pandas.read_csv("../output/output_heur6_3.000000.csv",sep = ";",header=2)
d_6_3["Instance name "] = d_6_3["Instance name "] + "_6channels"
d3 = pandas.concat([d_3_3,d_6_3])

d_3_4 = pandas.read_csv("../output/output_heur3_4.000000.csv",sep = ";",header=2)
d_3_4["Instance name "] = d_3_4["Instance name "] + "_3channels"
d_6_4 = pandas.read_csv("../output/output_heur6_4.000000.csv",sep = ";",header=2)
d_6_4["Instance name "] = d_6_4["Instance name "] + "_6channels"
d4 = pandas.concat([d_3_4,d_6_4])


def sum_of_costs():
    comparison = pandas.DataFrame()
    #comparison["Instance name"] = d15["Instance name "]
    comparison["1.2"] = d12['RH2 UB']
    comparison["1.5"] = d15['RH2 UB']
    comparison["2"] = d2['RH2 UB']
    comparison["3"] = d3['RH2 UB']
    comparison["4"] = d4['RH2 UB']
    d = comparison.sum()
    mean = d.mean()
    e = 100*(d-mean)/mean
    df = pandas.DataFrame()
    df['Sum of obj. value'] = d
    df['Spread to the mean (%)'] = e    
    df.to_csv("r_calibration/sum_of_costs.csv")
    

def comparison1v1UB(serie1,serie2,name_title,name_file):
    print("Descriptive statistics")
    mean = np.mean(100*(serie2-serie1)/serie1)
    print("Mean gap = {0}%".format(mean))
    mannwhitneyutest_result = scipy.stats.mannwhitneyu(serie1,serie2,alternative='greater')
    wilcoxontest_result = scipy.stats.wilcoxon(serie1,serie2,alternative='greater')
    print(mannwhitneyutest_result,wilcoxontest_result)
    serie1_3channel, serie1_6channel = serie1[:108], serie1[108:]
    serie2_3channel, serie2_6channel = serie2[:108], serie2[108:]
    savings_3channel = 100*(serie2_3channel-serie1_3channel)/serie1_3channel
    savings_6channel = 100*(serie2_6channel-serie1_6channel)/serie1_6channel
    plt.hist([savings_3channel,savings_6channel], bins=20,color=["grey",'black'],label = ["3 channels","6 channels"])
    plt.xlabel(name_title)
    plt.ylabel("Number of instances")
    plt.legend()
    plt.savefig("r_calibration/"+name_file+".png")
    plt.close()


sum_of_costs()

print("RH2 r=1.2 vs r=1.5")
comparison1v1UB(d12['RH2 UB'],d15['RH2 UB'],"Relative objective gap (%) from RH2 (1.2) to RH2 (1.5)","rh2_1#2_to_1#5_cost_comparison")
wilcoxontest_result_12_15 = scipy.stats.wilcoxon(d12['RH2 UB'],d15['RH2 UB'])
print("RH2 r=1.2 vs r=2")
comparison1v1UB(d12['RH2 UB'],d2['RH2 UB'],"Relative objective gap (%) from RH2 (1.2) to RH2 (2)","rh2_1#2_to_2_cost_comparison")
wilcoxontest_result_12_2 = scipy.stats.wilcoxon(d12['RH2 UB'],d2['RH2 UB'])
print("RH2 r=1.2 vs r=3")
comparison1v1UB(d12['RH2 UB'],d3['RH2 UB'],"Relative objective gap (%) from RH2 (1.2) to RH2 (3)","rh2_1#2_to_3_cost_comparison")
wilcoxontest_result_12_3 = scipy.stats.wilcoxon(d12['RH2 UB'],d3['RH2 UB'])
print("RH2 r=1.2 vs r=4")
comparison1v1UB(d12['RH2 UB'],d4['RH2 UB'],"Relative objective gap (%) from RH2 (1.2) to RH2 (4)","rh2_1#2_to_4_cost_comparison")
wilcoxontest_result_12_4 = scipy.stats.wilcoxon(d12['RH2 UB'],d4['RH2 UB'])
print("RH2 r=1.5 vs r=2")
comparison1v1UB(d15['RH2 UB'],d2['RH2 UB'],"Relative objective gap (%) from RH2 (1.5) to RH2 (2)","rh2_1#5_to_2_cost_comparison")
wilcoxontest_result_15_2 = scipy.stats.wilcoxon(d15['RH2 UB'],d2['RH2 UB'])
print("RH2 r=1.5 vs r=3")
comparison1v1UB(d15['RH2 UB'],d3['RH2 UB'],"Relative objective gap (%) from RH2 (1.5) to RH2 (3)","rh2_1#5_to_3_cost_comparison")
wilcoxontest_result_15_3 = scipy.stats.wilcoxon(d15['RH2 UB'],d3['RH2 UB'])
print("RH2 r=1.5 vs r=4")
comparison1v1UB(d15['RH2 UB'],d4['RH2 UB'],"Relative objective gap (%) from RH2 (1.5) to RH2 (4)","rh2_1#5_to_4_cost_comparison")
wilcoxontest_result_15_4 = scipy.stats.wilcoxon(d15['RH2 UB'],d4['RH2 UB'])
print("RH2 r=2 vs r=4")
comparison1v1UB(d2['RH2 UB'],d3['RH2 UB'],"Relative objective gap (%) from RH2 (2) to RH2 (3)","rh2_2_to_3_cost_comparison")
wilcoxontest_result_2_3 = scipy.stats.wilcoxon(d2['RH2 UB'],d3['RH2 UB'])
print("RH2 r=2 vs r=4")
comparison1v1UB(d2['RH2 UB'],d4['RH2 UB'],"Relative objective gap (%) from RH2 (2) to RH2 (4)","rh2_2_to_4_cost_comparison")
wilcoxontest_result_2_4 = scipy.stats.wilcoxon(d2['RH2 UB'],d4['RH2 UB'])
print("RH2 r=3 vs r=4")
comparison1v1UB(d3['RH2 UB'],d4['RH2 UB'],"Relative objective gap (%) from RH2 (3) to RH2 (4)","rh2_3_to_4_cost_comparison")
wilcoxontest_result_3_4 = scipy.stats.wilcoxon(d3['RH2 UB'],d4['RH2 UB'])











