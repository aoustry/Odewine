# -*- coding: utf-8 -*-
"""
Created on Fri Jan  7 19:14:22 2022

@author: aoust
"""

import pandas
import matplotlib.pyplot as plt
import scipy.stats
import numpy as np

d_3_RH = pandas.read_csv("../output_heur3_4.csv",sep = ";",header=2)
d_3_RH["Instance name "] = d_3_RH["Instance name "] + "_3channels"
d_6_RH = pandas.read_csv("../output_heur6_4.csv",sep = ";",header=2)
d_6_RH["Instance name "] = d_6_RH["Instance name "] + "_6channels"
d_RH = pandas.concat([d_3_RH,d_6_RH])

d_3_GH = pandas.read_csv("../output_greedy3.csv",sep = ";",header=1)
d_3_GH["Instance name "] = d_3_GH["Instance name "] + "_3channels"
d_6_GH = pandas.read_csv("../output_greedy6.csv",sep = ";",header=1)
d_6_GH["Instance name "] = d_6_GH["Instance name "] + "_6channels"
d_GH = pandas.concat([d_3_GH,d_6_GH])


mannwhitneyutest_result_GH = scipy.stats.mannwhitneyu(d_GH['GH1 UB'],d_GH['GH2 UB'],alternative='greater')
mannwhitneyutest_result_RH = scipy.stats.mannwhitneyu(d_RH['RH1 UB'],d_RH['RH2 UB'],alternative='greater')

wilcoxontest_result_GH = scipy.stats.wilcoxon(d_GH['GH1 UB'],d_GH['GH2 UB'],alternative='greater')
wilcoxontest_result_RH = scipy.stats.wilcoxon(d_RH['RH1 UB'],d_RH['RH2 UB'],alternative='greater')






#mannwhitneyutest_result__greedy_vs_relax = scipy.stats.mannwhitneyu(d_GH['GH2 UB'],d_RH[' H-inf-reg value '],alternative='greater')
mannwhitneyutest_result__relax_vs_greedy = scipy.stats.mannwhitneyu(d_RH['RH2 UB'],d_GH['GH2 UB'],alternative='greater')

#wilcoxontest_result__greedy_vs_relax = scipy.stats.wilcoxon(d_GH['GH2 UB'],d_RH[' H-inf-reg value '],alternative='greater')
wilcoxontest_result__relax_vs_greedy = scipy.stats.wilcoxon(d_RH['RH2 UB'],d_GH['GH2 UB'],alternative='greater')




def comparison1v1UB(serie1,serie2,name_title,name_file):
    print("Descriptive statistics")
    mean = np.mean(100*(serie1-serie2)/serie1)
    print("Mean gap = {0}%".format(mean))
    mannwhitneyutest_result = scipy.stats.mannwhitneyu(serie1,serie2,alternative='greater')
    wilcoxontest_result = scipy.stats.wilcoxon(serie1,serie2,alternative='greater')
    print(mannwhitneyutest_result,wilcoxontest_result)
    serie1_3channel, serie1_6channel = serie1[:108], serie1[108:]
    serie2_3channel, serie2_6channel = serie2[:108], serie2[108:]
    savings_3channel = 100*(serie1_3channel-serie2_3channel)/serie1_3channel
    savings_6channel = 100*(serie1_6channel-serie2_6channel)/serie1_6channel
    plt.hist([savings_3channel,savings_6channel], bins=20,color=["grey",'black'],label = ["3 channels","6 channels"])
    plt.xlabel(name_title)
    plt.ylabel("Number of instances")
    plt.legend()
    plt.savefig(name_file+".png")
    plt.close()
    
    

print("UB Greedy")
comparison1v1UB(d_GH['GH1 UB'],d_GH['GH2 UB'],"Relative cost reduction (%) from GH1 to GH2","greedy_cost_comparison")
print("Time Greedy")
#comparison1v1(d_GH['GH1 time']+0.001,d_GH['GH2 time'],-1,"Relative time augmentation (%) from GH1 to GH2","greedy_time_comparison")
print("UB Relax-based-heur")
comparison1v1UB(d_RH['RH1 UB'],d_RH['RH2 UB'],"Relative cost reduction (%) from RH1 to RH2","relax_based_heur_cost_comparison")
print("Time Relax-based-heur")
#comparison1v1(d_RH['RH1 time']+0.001,d_RH['RH2 time'],-1,"Relative time augmentation (%) from GH1 to GH2","relax_based_heur_time_comparison")
print('UB greedy vs relax')
comparison1v1UB(d_RH['RH2 UB'],d_GH['GH2 UB'],"Relative cost reduction (%) from RH2 to GH2","greedy_vs_relax_cost_comparison")