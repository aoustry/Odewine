 #include "CoverCutsMaker.h"
#include <vector>
#include <fstream>
#include <bits/stdc++.h>
using Iter = std::vector<int>::const_iterator;

CoverCutsMaker::CoverCutsMaker(){};
CoverCutsMaker::~CoverCutsMaker(){};
CoverCutsMaker::CoverCutsMaker(SceneInstance SI_entry, int i_entry, int j_entry, bool down_entry){
  SI = SI_entry;
  n = SI.n;
  m = SI.m;
  i = i_entry;
  j = j_entry;
  down = down_entry;

  if (down)
     threshold = (SI.p[j+n][i]/(pow(2,SI.d[i]) -1)) - SI.sigma;
  else
    threshold = (SI.p[i][j+n]/(pow(2,SI.u[i]) -1 )) - SI.sigma;

  assert(i < n);
  assert(j < m);

  // Construction de sorted_neighbours
  vector<tuple<float, int> > v;
	for (int idx = 0; idx<n+m; idx++){
    if ((idx!=i) and (idx!=j+n)){

      if (down)
         v.push_back(make_tuple(-SI.p[idx][i],idx));
      else
         v.push_back(make_tuple(-SI.p[idx][j+n],idx));
    }

	}

  sort(v.begin(), v.end());
  sorted_neighbours = {};
  int neigh ;
  tuple<float, int> t;
  for (int idx = 0; idx<v.size();idx++){
      t = v[idx];
      neigh = get<1>(t);
      sorted_neighbours.push_back(neigh);
  }

  //Détermination de kmax
  kmax = -1;
  float margin ;
  int idx = sorted_neighbours.size()-1;
  margin = threshold ;

  while ((margin>=0)and(idx>=0)){

    neigh = sorted_neighbours[idx];
    if (down)
      margin+= -SI.p[neigh][i];
    else
      margin+= - SI.p[neigh][j+n];
    kmax +=1;
    idx+=-1;
  }
  if (margin>=0){
    kmax+=1;
  }


  //Détermination de kmin
  kmin = 0;
  idx = 0;
  neigh = sorted_neighbours[idx];
  margin = threshold;
  while ((margin>=0)and(idx<sorted_neighbours.size())){
      neigh = sorted_neighbours[idx];
      if (down)
        margin+= -SI.p[neigh][i];
      else
        margin+= - SI.p[neigh][j+n];
      kmin +=1;
      idx+=1;
    }

  //cout <<"("<<kmin<<","<<kmax <<")\n";
}

void CoverCutsMaker::add_cut_coherent(IloEnv env, IloModel model, IloBoolVarArray x, IloBoolVarArray y, int k){
  // On exige que k soit strictemnt compris entre 0 et n+m-2
  assert(k>0);
  assert(k>=kmin);
  assert(k<=kmax);
  assert(k<n+m-2);

  float S = 0;
  int neigh_old;
  int neigh;
  for (int idx = 0; idx <k; idx++){
    neigh = sorted_neighbours[idx];
    if (down)
      S+= SI.p[neigh][i];
    else
      S+= SI.p[neigh][j+n];
    }
  int idx =k;

  while ((S>threshold)and(idx<=n+m-3)){
    neigh_old = sorted_neighbours[idx-k];
    neigh = sorted_neighbours[idx];
    if (down){
      S+= SI.p[neigh][i]-SI.p[neigh_old][i];
    }
    else{
      S+= SI.p[neigh][j+n]-SI.p[neigh_old][j+n];
    }
    idx+=1;
  }
  const int idx_max = idx -1 ;

  IloExpr expression(env);
  for(idx = 0 ; idx <=idx_max; idx++){
    neigh = sorted_neighbours[idx];
    if (neigh<n){//if client
      expression+=x[neigh]-y[neigh*m+j]; //x[neigh] - y[neigh,j]
    }
    else{
      expression+=x[neigh];
    }
  }

  model.add(expression<=(k-1)*y[i*m+j]+(idx_max+1)*(1-y[i*m+j]));
  expression.end();
  return;
}
