# -*- coding: utf-8 -*-
"""
Created on Fri Jan  7 19:14:22 2022

@author: aoust
"""

import pandas
import matplotlib.pyplot as plt
import scipy.stats


d_3_RH = pandas.read_csv("../output_heur3_4.csv",sep = ";",header=2)
d_3_RH["Instance name "] = d_3_RH["Instance name "] + "_3channels"
d_6_RH = pandas.read_csv("../output_heur6_4.csv",sep = ";",header=2)
d_6_RH["Instance name "] = d_6_RH["Instance name "] + "_6channels"
d_RH = pandas.concat([d_3_RH,d_6_RH])

d_3_GH = pandas.read_csv("../output_greedy3.csv",sep = ";",header=1)
d_3_GH["Instance name "] = d_3_GH["Instance name "] + "_3channels"
d_6_GH = pandas.read_csv("../output_greedy6.csv",sep = ";",header=1)
d_6_GH["Instance name "] = d_6_GH["Instance name "] + "_6channels"
d_GH = pandas.concat([d_6_GH,d_6_GH])


mannwhitneyutest_result_GH = scipy.stats.mannwhitneyu(d_GH['GH1 UB'],d_GH['GH2 UB'],alternative='greater')
mannwhitneyutest_result_RH = scipy.stats.mannwhitneyu(d_RH[' H-inf value'],d_RH[' H-inf-reg value '],alternative='greater')

wilcoxontest_result_GH = scipy.stats.wilcoxon(d_GH['GH1 UB'],d_GH['GH2 UB'],alternative='greater')
wilcoxontest_result_RH = scipy.stats.wilcoxon(d_RH[' H-inf value'],d_RH[' H-inf-reg value '],alternative='greater')
