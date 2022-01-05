# -*- coding: utf-8 -*-
"""
Created on Wed Nov  7 19:58:12 2018

@author: oustry
"""


from __future__ import division

import math, os,time
import numpy as np
import pandas,numpy,operator


class InstanceBuilder():
    
    
    def __init__(self, mapname,freq_index,nodes_index,program_param_index):
        
        
        self.gain =  pandas.read_csv(mapname+"_GainMatrix{0}_{1}.csv".format(freq_index, nodes_index),index_col=0).values
            
        ################# Model parameters #######################
        DFCL = pandas.read_csv(mapname+"_CL_"+str(nodes_index)+".csv",index_col=0).values
        DFCA = pandas.read_csv(mapname+"_CA_"+str(nodes_index)+".csv",index_col=0).values
        print(DFCA)
        self.n,self.m= len(DFCL),len(DFCA)
        assert(self.gain.shape == (self.n+self.m,self.n+self.m))
       
        self.c =  DFCA[:,2]
        self.u = DFCL[:,2]
        self.d = DFCL[:,3]
        
        DFP = (pandas.read_csv("Lib/Param_"+str(program_param_index)+".csv",index_col=0)).values.astype('float')
        
        S_clients, S_cand, self.gamma, self.sigma, self.LAMBDA = DFP[:,0]
       
        S = [S_clients for i in range(self.n)] + [S_cand for i in range(self.m)]
        self.p= numpy.zeros((self.n+self.m,self.n +self.m))        
        
        for i in range(self.n +self.m):
            
            for j in range(self.n + self.m):
            
                self.p[i][j] = self.gain[i][j] * S[i]
        self.fname = "Instance_"+mapname[4:]+freq_index+"_"+str(nodes_index)+"_"+str(program_param_index)
        
        
    
    def matrix_print(self,m):
        
        """ Only used to define matrix on V = V_clients union V_cand"""
        
        for i in range(m.shape[0]):
            
            for j in range(m.shape[1]):
                
                self.data_file.write(self.dec_writing(m[i][j])+"\n")
                
    def vector_print(self,v):
        
        """The offset enables to define a vector on V_client or V_cand or V_r"""
        
        for i in range(len(v)):
            
            self.data_file.write(self.dec_writing(v[i])+"\n")
    
    def dec_writing(self, number):
        
        """ Function to avoid scientific writing """
        
        s=str(int(number//1))
        s=s+"."

                
        for i in range(20) :
            number=number*10
            s=s+str(int(number%10))
            
            
        return s
    
    
    def build_file(self):
        """Method building DAT File """
        
        ###############  File creation ###########################
        self.data_file=open("optim_instances/"+self.fname+".dat","w+")
                
        ############### Cardinals ################################
        self.data_file.write(str(self.n)+"\n")
        self.data_file.write(str(self.m)+"\n")
        self.data_file.write(str(self.LAMBDA)+"\n")
        self.data_file.write(str(self.gamma)+"\n")
        self.data_file.write(self.dec_writing(self.sigma)+"\n")
        
              
        
        ############### Vectors #############################
        self.vector_print(self.c)
        self.vector_print(self.d)
        self.vector_print(self.u)

	############### Power matrix #############################
        self.matrix_print(self.p)

        self.data_file.close()


MAPLIST = ["Lib/MAP1","Lib/MAP2","Lib/MAP3","Lib/MAP4","Lib/MAP5","Lib/MAP6"]

for mapname in MAPLIST:
    for i in range(3):
        for j in [1,2,3]:
            I = InstanceBuilder(mapname,"A",i,j)
            I.build_file()
            I = InstanceBuilder(mapname,"B",i,j)
            I.build_file()
     
