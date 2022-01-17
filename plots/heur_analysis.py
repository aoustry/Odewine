# -*- coding: utf-8 -*-
"""
Created on Fri Jan  7 19:14:22 2022

@author: aoust
"""

import pandas
import matplotlib.pyplot as plt
import scipy.stats
import numpy as np

d_3_RH = pandas.read_csv("../output/output_heur3_3.000000.csv",sep = ";",header=2)
d_3_RH["Instance name "] = d_3_RH["Instance name "] + "_3channels"
d_6_RH = pandas.read_csv("../output/output_heur6_3.000000.csv",sep = ";",header=2)
d_6_RH["Instance name "] = d_6_RH["Instance name "] + "_6channels"
d_RH = pandas.concat([d_3_RH,d_6_RH])

d_3_GH = pandas.read_csv("../output/output_greedy3.csv",sep = ";",header=1)
d_3_GH["Instance name "] = d_3_GH["Instance name "] + "_3channels"
d_6_GH = pandas.read_csv("../output/output_greedy6.csv",sep = ";",header=1)
d_6_GH["Instance name "] = d_6_GH["Instance name "] + "_6channels"
d_GH = pandas.concat([d_3_GH,d_6_GH])



def histogram(serie):
    serie = list(serie)
    serie.sort()
    L = len(serie)
    return serie, list(range(1,1+L))


def comparison1v1UB(serie1,serie2,name_title,name_file):
    print("Descriptive statistics")
    mean = np.mean(100*(serie1-serie2)/serie1)
    std = np.std(100*(serie1-serie2)/serie1)
    print("Mean improvement = {0}%".format(mean))
    print("Std improvement = {0}%".format(std))
    mannwhitneyutest_result = scipy.stats.mannwhitneyu(serie1,serie2)
    wilcoxontest_result = scipy.stats.wilcoxon(serie1,serie2)
    print(mannwhitneyutest_result,wilcoxontest_result)
    serie1_3channel, serie1_6channel = serie1[:108], serie1[108:]
    serie2_3channel, serie2_6channel = serie2[:108], serie2[108:]
    delta_3channels = 100*(serie1_3channel-serie2_3channel)/serie1_3channel
    delta_6channels = 100*(serie1_6channel-serie2_6channel)/serie1_6channel
    delta = 100*(serie1-serie2)/serie1
    print("Quantiles delta = {0}".format(delta.quantile([0,0.25,0.5,0.75,0.8,0.85,0.9,1])))
    plt.hist([delta_3channels,delta_6channels], bins=20,color=["grey",'black'],label = ["3 channels","6 channels"])
    plt.xlabel(name_title)
    plt.ylabel("Number of instances")
    plt.legend()
    plt.savefig(name_file+".png")
    plt.close()


def time_distribution(serie1,serie2,name):
    L = len(serie1)
    M = max(serie1.max(),serie2.max())
    x1,y1 = histogram(serie1+0.001)
    x2,y2 = histogram(serie2+0.001)
    plt.plot(x1+[M],y1+[L], color = 'black', linestyle='--', label = name+'1')
    plt.plot(x2+[M],y2+[L], color = 'black',  linestyle='-', label = name+'2')
    
    plt.xscale('log')
    plt.legend()
    plt.xlabel("Time (s)")
    plt.ylabel("Number of instances solved within this time")
    plt.savefig(name+"TimeDistribution.png")
    plt.close()
    
def comparison_scatter_gap_and_size(serie1,serie2,size,name_title,name_file):
       
    delta = 100*(serie1-serie2)/serie1
    print(scipy.stats.pearsonr(size,delta))
    plt.scatter(size[:108],delta[:108],color="grey",label = "3 channels")
    plt.scatter(size[108:],delta[108:],color="black",label = "6 channels")
    plt.xlabel("Instance size |I| + |J|")
    plt.ylabel(name_title)
    plt.legend()
    plt.savefig(name_file+".png")
    plt.close()
    
def comparison1v1time(serie1,serie2,name_title,name_file):
    print("Descriptive statistics")
    median = np.median((serie2-serie1))
    mean = np.mean((serie2-serie1))
    std = np.std((serie2-serie1))
    print("Max s1 = {0}s".format(serie1.max()))
    print("Max s2 = {0}s".format(serie2.max()))
    print("median gap = {0}s".format(median))
    print("Mean gap = {0}s".format(mean))
    print("Std gap = {0}s".format(std))
    mannwhitneyutest_result = scipy.stats.mannwhitneyu(serie2,serie1,alternative='greater')
    wilcoxontest_result = scipy.stats.wilcoxon(serie2,serie1,alternative='greater')
    print(mannwhitneyutest_result,wilcoxontest_result)
    serie1_3channel, serie1_6channel = serie1[:108], serie1[108:]
    serie2_3channel, serie2_6channel = serie2[:108], serie2[108:]
    delta_3channels = (serie2_3channel-serie1_3channel)
    delta_6channels = (serie2_6channel-serie1_6channel)
    plt.hist([delta_3channels,delta_6channels], bins=20,color=["grey",'black'],label = ["3 channels","6 channels"])
    plt.xlabel(name_title)
    plt.ylabel("Number of instances")
    plt.legend()
    plt.savefig(name_file+".png")
    plt.close()

def comparison1v1UBscatter(serie1,serie2,name_xtitle,name_ytitle,name_file):
    m = max(serie1.max(),serie2.max())
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
    

print("--------------UB Greedy--------------")
comparison1v1UB(d_GH['GH1 UB'],d_GH['GH2 UB'],"Objective improvement (%) from GH1 to GH2","greedycostcomparison")
comparison1v1UBscatter(d_GH['GH1 UB'],d_GH['GH2 UB'],"Objective value (GH1)","Objective value (GH2)","scatter_greedy")
comparison_scatter_gap_and_size(d_GH['GH1 UB'],d_GH['GH2 UB'],(d_GH['|I| ']+d_GH['|J|']).values,"Objective improvement (%) from GH1 to GH2","scattergreedygapvssize")

print("--------------Time Greedy--------------")
comparison1v1time(d_GH['GH1 time'],d_GH['GH2 time'],"Absolute time difference (s) from GH1 to GH2","greedytimecomparison")
time_distribution(d_GH['GH1 time'],d_GH['GH2 time'],"GH")

print("--------------UB Relax-based-heur--------------")
comparison1v1UB(d_RH['RH1 UB'],d_RH['RH2 UB'],"Objective improvement (%) from RH1 to RH2","relaxbasedheurcostcomparison")
comparison1v1UBscatter(d_RH['RH1 UB'],d_RH['RH2 UB'],"Objective value (RH1)","Objective value (RH2)","scatter_relbased")
comparison_scatter_gap_and_size(d_RH['RH1 UB'],d_RH['RH2 UB'],(d_RH['|I| ']+d_RH['|J|']).values,"Objective improvement (%) from RH1 to RH2","scatterrelbasedgapvssize")

print("--------------Time Relax-based-heur--------------")
comparison1v1time(d_RH['RH1 time'],d_RH['RH2 time'],"Absolute time difference (s) from RH1 to RH2","relaxbasedheurtimecomparison")
time_distribution(d_RH['RH1 time'],d_RH['RH2 time'],"RH")
