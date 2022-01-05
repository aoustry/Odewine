CC = g++
CFLAGS = -std=c++11

#---------------------------------------------------------------------------
#Code below searches for cplex install dir. If you know it, you can erase all
#lines below (up to first comment) and just put a line as follows.
#CPLEX=your/cplex/folder/
#Otherwise let it check for several folders and it will set the first existing
#one. If all fail, the compilation will halt before trying to build main
ifneq ($(wildcard /data/cplex128/),)  #check if this folder exists
    CPLEX   = /data/cplex128/
endif 
ifneq ($(wildcard /opt/ibm/ILOG/),)            
    #Cplex is often installed in a subdir of above folder.
    #take last subdir inside (ex, CPLEX_Studio...) as it can be the last version
    CPLEXVER= $(shell ls /opt/ibm/ILOG/|tail -1)
    CPLEX   = /opt/ibm/ILOG/${CPLEXVER}
endif 
#If possible I always take version 12.8
ifneq ($(wildcard /opt/ibm/ILOG/CPLEX_Studio128/),) 
    CPLEX   = /opt/ibm/ILOG/CPLEX_Studio128
endif 


#----------------------------------------------------------------------------
#The rest depends on the cplex install dire defined above
CPLEX = /usr/local/ilog
CPLEXDIR      = $(CPLEX)/cplex/
CONCERTDIR    = $(CPLEX)/concert/

# ---------------------------------------------------------------------
# Compiler selection, code optimization, debug and warning options
# ---------------------------------------------------------------------
#use DNDEBUG to remove asserts and turn off some clog debug messages
CCFLAGS = -O3 -m64 -Wall -Wno-ignored-attributes -g
#maybe add -Wextra and -Wpedantic
#maybe -Ofast that enables -ffast-math. 
#For me, the following can be good enough instead of -Ofast. However, it
#assumes things like floating point multiplication is associative, untrue.
#-funsafe-math-optimizations -ffinite-math-only


# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------
LIBFORMAT  = static_pic
#cplex related
#use SYSTEM=x86-64_sles10_4.1 for cplex versions <12.6
SYSTEM     = x86-64_linux

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CCLNFLAGS =" " 
CCLNFLAGSCPLEX = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lrt -lpthread -ldl
               #-m32 -pthread -lm (add more when needed)

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCFLAGSCPLEX = $(CCFLAGS) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)  -DIL_STD #DIL_STD: CPLEX specific macro

# ---------------------------------------------------------------------
# Main compilation and linking commands
# ---------------------------------------------------------------------



all: cplexdirexists main_milp

cplexdirexists:
	echo "Dossier cplex : $(CPLEX)"

main_milp: main_milp.cpp
	g++ $(CCFLAGSCPLEX) main_milp.cpp -o main_milp $(CCLNFLAGSCPLEX)  $(CFLAGS)

clean:
	rm -rf *.o


