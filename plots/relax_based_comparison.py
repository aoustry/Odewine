# -*- coding: utf-8 -*-
"""
Created on Fri Jan  7 10:38:13 2022

@author: aoust
"""

import pandas
import matplotlib.pyplot as plt
import scipy.stats
d_3_15 = pandas.read_csv("../output_heur3.csv",sep = ";",header=2)
d_3_15["Instance name "] = d_3_15["Instance name "] + "_3channels"
d_6_15 = pandas.read_csv("../output_heur6.csv",sep = ";",header=2)
d_6_15["Instance name "] = d_6_15["Instance name "] + "_6channels"
d15 = pandas.concat([d_3_15,d_6_15])

d_3_2 = pandas.read_csv("../output_heur3_2.csv",sep = ";",header=2)
d_3_2["Instance name "] = d_3_2["Instance name "] + "_3channels"
d_6_2 = pandas.read_csv("../output_heur6_2.csv",sep = ";",header=2)
d_6_2["Instance name "] = d_6_2["Instance name "] + "_6channels"
d2 = pandas.concat([d_3_2,d_6_2])

d_3_4 = pandas.read_csv("../output_heur3_4.csv",sep = ";",header=2)
d_3_4["Instance name "] = d_3_4["Instance name "] + "_3channels"
d_6_4 = pandas.read_csv("../output_heur6_4.csv",sep = ";",header=2)
d_6_4["Instance name "] = d_6_4["Instance name "] + "_6channels"
d4 = pandas.concat([d_3_4,d_6_4])


def plots():
    a = 100*(d2[' H-inf-reg value '] - d15[' H-inf-reg value '])/(d15[' H-inf-reg value ']+d2[' H-inf-reg value ']+d4[' H-inf-reg value '])
    b = 100*(d2[' H-inf-reg value '] - d4[' H-inf-reg value '])/(d15[' H-inf-reg value ']+d2[' H-inf-reg value ']+d4[' H-inf-reg value '])
    c = 100*(d4[' H-inf-reg value '] - d15[' H-inf-reg value '])/(d15[' H-inf-reg value ']+d2[' H-inf-reg value ']+d4[' H-inf-reg value '])
    #c = d2[' H-inf-reg value '] - d4[' H-inf-reg value ']
    hx2_to4, hy2_to4, _ = plt.hist((b[b!=0]), bins=20,color="grey")
    plt.xlabel("Relative gap (%)")
    plt.ylabel("Number of instances")
    plt.savefig("RELAXHEUR_savings_param2_to_4.png")
    plt.close()
    hx2_to15, hy2_to15, _ = plt.hist((a[a!=0]), bins=20,color="grey")
    plt.xlabel("Relative gap (%)")
    plt.ylabel("Number of instances")
    plt.savefig("RELAXHEUR_savings_param2_to_1#5.png")
    plt.close()
    atilde,btilde,ctilde = a[a!=0],b[b!=0],c[c!=0]
    print(3*a.mean(),3*atilde.median())
    print(3*b.mean(),3*btilde.median())
    print(3*c.mean(),3*ctilde.median())


comparison = pandas.DataFrame()
comparison["Instance name"] = d15["Instance name "]
comparison["1.5"] = d15[' H-inf-reg value ']
comparison["2"] = d2[' H-inf-reg value ']
comparison["4"] = d4[' H-inf-reg value ']

#Argument no significant statistical difference between the different parameter values.
mannwhitneyutest_result_15_2 = scipy.stats.mannwhitneyu(d15[' H-inf-reg value '],d2[' H-inf-reg value '])
mannwhitneyutest_result_15_4 = scipy.stats.mannwhitneyu(d15[' H-inf-reg value '],d4[' H-inf-reg value '])
mannwhitneyutest_result_2_4 = scipy.stats.mannwhitneyu(d2[' H-inf-reg value '],d4[' H-inf-reg value '])

wilcoxontest_result_15_2 = scipy.stats.wilcoxon(d15[' H-inf-reg value '],d2[' H-inf-reg value '])
wilcoxontest_result_15_4 = scipy.stats.wilcoxon(d15[' H-inf-reg value '],d4[' H-inf-reg value '])
wilcoxontest_result_2_4 = scipy.stats.wilcoxon(d2[' H-inf-reg value '],d4[' H-inf-reg value '])