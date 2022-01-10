#include <iostream>
#include <vector>
#include <fstream>
#include <bits/stdc++.h>
#include "milp.cpp"
#include <cstdlib>
#include <assert.h>
#include <ilcplex/ilocplex.h>

using namespace std::chrono;


float * solve_coherent_no_noise(SceneInstance SI, int * pattern, vector<tuple<int, int> > &edges,float TL)
{
  assert(edges.size()==0);
  auto start = high_resolution_clock::now();
    	IloInt n = SI.n;
    	IloInt m = SI.m;

	IloEnv env = IloEnv();

	//Variables
	IloBoolVarArray x(env, n+m);
	IloBoolVarArray y(env, n*m);

	//Modele
	IloModel model(env);

	//Objectif
	IloExpr obj_expr(env);
	for (int client=0; client<n; client++)
	{
		obj_expr += SI.lambda*(SI.d[client]+SI.u[client]) * (1-x[client]);
	}
	for (int cand=0; cand<m; cand++)
	{
		obj_expr += SI.c[cand]*x[cand+n];
	}
	model.add(IloMinimize(env, obj_expr));
	obj_expr.end();

	// Contraintes

	//1- At most one channel per client
	for (int client=0; client<n; client++)
	{	IloExpr sum_client(env);
		for (int cand=0; cand<m; cand++)
		{
			sum_client += y[client*m+cand];
			model.add(y[client*m+cand] <= x[client]);
			model.add(y[client*m+cand] <= x[cand+n]);
		}

		model.add(sum_client == x[client]);
		sum_client.end();
	}

	//2- AP capacity
	for (int cand=0; cand<m; cand++)
	{	IloExpr sum_cand(env);
		for (int client=0; client<n; client++)
		{
			sum_cand += (SI.d[client]+SI.u[client])*y[client*m+cand];
		}

		model.add(sum_cand <= SI.gamma * x[cand+n]);
		sum_cand.end();
	}

  for (int cand=0; cand<m; cand++)
  {
    for (int client=0; client<n; client++)
    {
      if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma))
      {
        model.add(y[client*m+cand] ==0);

      }
    }
  }

	IloCplex cplex(model);

	// Resolution
  cplex.setParam(IloCplex::Param::TimeLimit,TL);
  	cplex.setOut(env.getNullStream());

	cplex.solve();
  IloNumArray patt_aux(env);
  cplex.getValues(x, patt_aux);
  for (int k =0; k<n+m;k++){
    pattern[k]=patt_aux[k];
    assert(pattern[k]==round(pattern[k]));
  }
  for (int client=0; client<n; client++)
    for (int cand = 0; cand <m; cand++)
      if (cplex.getValue(y[client*m+cand])>0.001)
        edges.push_back(make_tuple(client,cand));
	x.end();
	y.end();
  auto end = high_resolution_clock::now();
  float time_elapsed = (duration_cast<milliseconds>(end-start)).count();
  static float result[2];
  result[0] = cplex.getObjValue() ;
  result[1] = time_elapsed/1000 ;
env.end();

    return result ;



}

bool solve_coherent_no_noise_reg(SceneInstance SI, int * pattern, vector<tuple<int, int> > &edges,float * mu, int ref, float TL)
{
  assert(edges.size()==0);
  auto start = high_resolution_clock::now();
    	IloInt n = SI.n;
    	IloInt m = SI.m;

	IloEnv env = IloEnv();

	//Variables
	IloBoolVarArray x(env, n+m);
  IloNumVarArray s(env, n+m,0,1000000);
	IloBoolVarArray y(env, n*m);

	//Modele
	IloModel model(env);

	//Objectif
  IloExpr cost_expr(env);
	IloExpr obj_expr(env);


	for (int client=0; client<n; client++)
	{
		cost_expr += SI.lambda*(SI.d[client]+SI.u[client]) * (1-x[client]);
    obj_expr += mu[client]*s[client];
	}

	for (int cand=0; cand<m; cand++)
	{

		cost_expr += SI.c[cand]*x[cand+n];
    obj_expr += mu[cand+n]*s[cand+n];
	}


	model.add(IloMinimize(env, obj_expr));
  assert(ref>0);
  model.add(cost_expr<=ref);
	obj_expr.end();

	// Contraintes
  //0- contraintes si
  for (int client = 0 ; client < n ; client ++){
    model.add(s[client]>=0);
    IloExpr sum_client(env);
    for (int cand = 0 ; cand <m;cand++){
      sum_client+=SI.p[cand+n][client]*(x[cand+n]-y[client*m+cand]);
    //   for (int client2 = 0 ; client2 <n ; client2 ++){
    //     if (client !=client2){
    //       sum_client+=SI.p[client2][client]*(y[client2*m+cand]-y[client*m+cand]);
    //     }
    // }

    model.add(IloIfThen(env,(x[client]==1),sum_client<=s[client] ));

    }
  }
  for (int cand = 0 ; cand <m ; cand++){
    model.add(s[cand+n]>=0);
    IloExpr sum_cand(env);
    for (int cand2 = 0 ;cand2 <m ; cand2++){
      if (cand!=cand2){
        sum_cand+= SI.p[n+cand2][n+cand]*x[cand2+n];
      }
    }
    for (int client = 0 ; client <n ; client ++){
      sum_cand+= SI.p[client][cand+n]*(x[client]-y[client*m+cand]);
    }
      model.add(IloIfThen(env,(x[cand+n]==1),sum_cand<=s[cand+n] ));
  }


	//1- At most one channel per client
	for (int client=0; client<n; client++)
	{	IloExpr sum_client(env);
		for (int cand=0; cand<m; cand++)
		{
			sum_client += y[client*m+cand];
			model.add(y[client*m+cand] <= x[client]);
			model.add(y[client*m+cand] <= x[cand+n]);
		}

		model.add(sum_client == x[client]);
		sum_client.end();
	}

	//2- AP capacity
	for (int cand=0; cand<m; cand++)
	{	IloExpr sum_cand(env);
		for (int client=0; client<n; client++)
		{
			sum_cand += (SI.d[client]+SI.u[client])*y[client*m+cand];
		}

		model.add(sum_cand <= SI.gamma * x[cand+n]);
		sum_cand.end();
	}

  for (int cand=0; cand<m; cand++)
  {
    for (int client=0; client<n; client++)
    {
      if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma))
      {
        model.add(y[client*m+cand] ==0);

      }
    }
  }

	IloCplex cplex(model);

	// Resolution
  cplex.setParam(IloCplex::Param::TimeLimit,TL);
  cplex.setParam(IloCplex::Param::Threads,4);
  cplex.setOut(env.getNullStream());
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);


  cplex.solve();
  bool result;
  try{
  IloNumArray patt_aux(env);
  cplex.getValues(x, patt_aux);
  for (int k =0; k<n+m;k++){
    pattern[k]=patt_aux[k];
    assert(pattern[k]==round(pattern[k]));
  }
  for (int client=0; client<n; client++)
    for (int cand = 0; cand <m; cand++)
      if (cplex.getValue(y[client*m+cand])>0.001)
        edges.push_back(make_tuple(client,cand));
	result = true;
    }
 catch(...){
	result = false;
}

x.end();
	y.end();
env.end();
    return result ;



}




float solve_FA(SceneInstance SI, int * pattern, vector<tuple<int, int> > edges, float TL, int C,int * op, int * opc)
{
  auto start = high_resolution_clock::now();
  IloInt n = SI.n;
  IloInt m = SI.m;
  assert(edges.size()>0);
	IloEnv env = IloEnv();

	//Variables
	IloBoolVarArray x(env, (n+m));
  IloBoolVarArray xc(env, (n+m)*C);

	//Modele
	IloModel model(env);

	//Objectif
	IloExpr obj_expr(env);
	for (int client=0; client<n; client++)
	{
		obj_expr += SI.lambda*(SI.d[client]+SI.u[client]) * (1-x[client]);
	}
	for (int cand=0; cand<m; cand++)
	{
		obj_expr += SI.c[cand]*x[cand+n];
	}
	model.add(IloMinimize(env, obj_expr));
	obj_expr.end();

	// Contraintes
  //0- respect pattern
  for (int k = 0 ; k<n+m;k++)
      model.add(x[k]<=pattern[k]);

	//1- At most one channel per device
	for (int k=0; k<n+m; k++)
	{	IloExpr sum_device(env);
		for (int color=0; color<C; color++)
		{
			sum_device += xc[k+color*(n+m)];
		}
		model.add(sum_device == x[k]);
		sum_device.end();
	}

  int cand; int client ; tuple <int,int> t;
  int cand2; int client2 ; tuple <int,int> t2;

  for (int color=0; color<C; color++){
  	for (int ind = 0; ind <edges.size();ind++){
      t = edges[ind];
      client = get<0>(t);cand = get<1>(t);
      //Client on if AP on
      model.add(xc[client+color*(n+m)]<=xc[cand+n+color*(n+m)]);
      //Contraintes d'interférence
  			assert(((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) >= SI.sigma) and (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) >= SI.sigma)));

  				IloExpr noise_client(env);
  				IloExpr noise_cand(env);
  				noise_client += SI.sigma;
  				noise_cand += SI.sigma;
          //Noise from other APs
  				for (int k=0; k<m; k++){
  					if (k!=cand){
  						noise_client+= SI.p[k+n][client]*xc[k+n+color*(n+m)];
  						noise_cand += SI.p[k+n][cand+n]*xc[k+n+color*(n+m)];
  					}
          }
          //Noise from other clients not in the same group
          for (int ind2 = 0; ind2 <edges.size();ind2++){
            t2 = edges[ind2];
            client2 = get<0>(t2);cand2 = get<1>(t2);
            if (cand2!=cand){
              noise_client+=SI.p[client2][client]*xc[client2+color*(n+m)];
              noise_cand+=SI.p[client2][cand+n]*xc[client2+color*(n+m)];
            }
  				}
  			model.add(IloIfThen(env,(xc[client+color*(n+m)]==1), noise_client <= SI.p[cand+n][client]/(pow(2,SI.d[client]) -1)));
  			model.add(IloIfThen(env,(xc[client+color*(n+m)]==1), noise_cand <=  SI.p[client][cand+n]/(pow(2,SI.u[client]) -1 )));
  			noise_client.end();
  			noise_cand.end();
  		}
  	}

	IloCplex cplex(model);

	// Resolution
  cplex.setParam(IloCplex::Param::TimeLimit,TL);
  cplex.setParam(IloCplex::Param::Threads,4);
  	//cplex.setOut(env.getNullStream());
  //cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);
	cplex.solve();
  IloNumArray patt_aux(env);
  IloNumArray patt_aux_c(env);
  cplex.getValues(x, patt_aux);
  cplex.getValues(xc, patt_aux_c);
  for (int k =0; k<n+m;k++){
    op[k]=patt_aux[k];
    assert(op[k]==round(op[k]));
  }
  for (int k =0; k<C*(n+m);k++){
    opc[k]=patt_aux_c[k];
    assert(opc[k]==round(opc[k]));
  }
  float val = cplex.getObjValue();
  cout<<val<<"\n";
  // cout<<cplex.getBestObjValue()<<"\n";
	x.end();
  xc.end();
  auto end = high_resolution_clock::now();
  float time_elapsed = (duration_cast<milliseconds>(end-start)).count();
  // float result[2];
  // result[0] = cplex.getObjValue() ;
  // result[1] = time_elapsed/1000 ;
	env.end();
  return val;
  ///return cplex.getObjValue() ;
}

float complete_clients(SceneInstance SI, float TL, int C, int * xstarc)
{
  auto start = high_resolution_clock::now();
  IloInt n = SI.n;
  IloInt m = SI.m;

	IloEnv env = IloEnv();

	//Variables
	IloBoolVarArray x(env, (n+m));
  IloBoolVarArray xc(env, (n+m)*C);
	IloBoolVarArray yc(env, n*m*C);

	//Modele
	IloModel model(env);

	//Objectif
	IloExpr obj_expr(env);
	for (int client=0; client<n; client++)
	{
		obj_expr += SI.lambda*(SI.d[client]+SI.u[client]) * (1-x[client]);
	}
	for (int cand=0; cand<m; cand++)
	{
		obj_expr += SI.c[cand]*x[cand+n];
	}
	model.add(IloMinimize(env, obj_expr));
	obj_expr.end();

	// Contraintes
  for (int i = 0; i<n;i++){
    for (int indc = 0 ; indc < C; indc++){
      if (xstarc[i+indc*(n+m)]==1){
      model.add(xc[i+indc*(n+m)]==1);
    }
    }
  }
  for (int i = n; i<n+m;i++){
    for (int indc = 0 ; indc < C; indc++){
    model.add(xc[i+indc*(n+m)]==xstarc[i+indc*(n+m)]);
  }
  }

	//1- At most one channel per client
	for (int k=0; k<n+m; k++)
	{	IloExpr sum_device(env);
		for (int color=0; color<C; color++)
		{
			sum_device += xc[k+color*(n+m)];

		}

		model.add(sum_device == x[k]);
		sum_device.end();
	}

  for (int color=0; color<C; color++){

  //2 - Every client comm with one ap
  for (int client=0; client<n; client++)
	{	IloExpr sum_client(env);
		for (int cand=0; cand<m; cand++) {
      sum_client+=yc[client*m+cand+color*(m*n)];
    }
    model.add(sum_client == xc[client + color*(m+n)]);
    sum_client.end();
  }

	//3- AP capacity and channel tuning
	for (int cand=0; cand<m; cand++)
	{	IloExpr sum_cand(env);
		for (int client=0; client<n; client++){
    	model.add(yc[client*m+cand+color*(m*n)]<=xc[client + color*(m+n)]);
      model.add(yc[client*m+cand+color*(m*n)]<=xc[cand + n + color*(m+n)]);

			sum_cand += (SI.d[client]+SI.u[client])*yc[client*m+cand+color*(m*n)];
		}

		model.add(sum_cand <= SI.gamma * xc[cand+n+color*(n+m)]);
		sum_cand.end();
	}


	//Contraintes d'interférence

	for (int cand=0; cand<m; cand++)
	{
		for (int client=0; client<n; client++)
		{
			if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma))
			{
				model.add(yc[client*m+cand+color*(m*n)] ==0);

			}
			else
			{ 	//sigma +sum{k in V_cand,k2 in V_clients :k<>jand k2<>i}(p[k2,i]*y[k2,k]) + sum{ k in V_cand :k<>j}(p[k,i]*x[k])
				IloExpr noise_client(env);
				IloExpr noise_cand(env);
				noise_client += SI.sigma;
				noise_cand += SI.sigma;
				for (int k=0; k<m; k++){

					if (k!=cand){
						noise_client+= SI.p[k+n][client]*xc[k+n+color*(n+m)];
						noise_cand += SI.p[k+n][cand+n]*xc[k+n+color*(n+m)];
						for (int k2 =0; k2<n;k2++){
							if (k2!=client){
							noise_client+= SI.p[k2][client]*yc[k2*m+k+color*(n*m)];
							noise_cand+= SI.p[k2][cand+n]*yc[k2*m+k+color*(n*m)];
							}
						}


					}
				}
			model.add(IloIfThen(env,(yc[client*m+cand+color*(n*m)]==1), noise_client <= SI.p[cand+n][client]/(pow(2,SI.d[client]) -1)));
			model.add(IloIfThen(env,(yc[client*m+cand+color*(n*m)]==1), noise_cand <=  SI.p[client][cand+n]/(pow(2,SI.u[client]) -1 )));
			noise_client.end();
			noise_cand.end();


			}
		}

  }
	}


	IloCplex cplex(model);

	// Resolution
  cplex.setParam(IloCplex::Param::TimeLimit,TL);
  cplex.setParam(IloCplex::Param::Threads,4);
  cplex.setOut(env.getNullStream());
  //cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);

	cplex.solve();

	x.end();
  	xc.end();
	yc.end();
        cout<<"Hello \n";
  	auto end = high_resolution_clock::now();
  	float time_elapsed = (duration_cast<milliseconds>(end-start)).count();
	cout<<cplex.getCplexStatus();
	float res = cplex.getObjValue() ;
	env.end();

  return res;


}

void RH1andRH2(SceneInstance SI, float innerTL, int C, float globalTL, float perturbation, std::string fname){
  bool bool_complete = true;
  auto start = high_resolution_clock::now();
  int patt [SI.n+SI.m];
  vector<tuple<int, int> > edges ={};
  float * stat0 = solve_coherent_no_noise(SI, patt , edges, innerTL);
  cout <<"Relaxation solved \n";
  int ref = round(stat0[0]);
  int output_pattern[SI.n+SI.m];
  int output_pattern_c[C*(SI.n+SI.m)];
  int val = round(solve_FA(SI, patt,edges, innerTL, C,output_pattern,output_pattern_c));
   cout <<"FA solved \n";
  int val_init = val;
  auto end1 = high_resolution_clock::now();
  int valmin = val; int val2;
  float mu [SI.n+SI.m]; int k = 0 ;
  for (int i = 0 ; i<SI.n+SI.m ; i++){
    mu[i] = 1;
  }
  int delta = 100000;
  for (int client = 0; client<SI.n;client++){
	delta = min(delta, (int) round(SI.lambda*(SI.d[client]+SI.u[client])));
	}

for (int cand=0; cand<SI.m; cand++)
	{
		delta = min(delta, (int) round(SI.c[cand]));
	}  cout <<"delta : "<< delta <<"\n";
  auto start_2 = high_resolution_clock::now();
  auto end = high_resolution_clock::now();
  while ((ref<valmin)and ((duration_cast<milliseconds>(end-start_2)).count() < globalTL) ) {
    k++;
    for (int i = 0 ; i<SI.n+SI.m ; i++){
      assert(patt[i]>=output_pattern[i]);
      if (patt[i]-output_pattern[i]>0.5){
          mu[i] = perturbation*mu[i];
        }
    }
    edges = {};
    int to = 60;
    bool continuing = false;
    while ((continuing ==false) and ((duration_cast<milliseconds>(end-start_2)).count() < globalTL)){

      continuing = solve_coherent_no_noise_reg(SI, patt, edges,mu,ref, to);
      if (continuing ==false){
         to = min(3600,to*2);
    }
}
  

  cout<<"FA begin \n";
  val = round(solve_FA(SI, patt,edges, innerTL, C,output_pattern,output_pattern_c));
  cout<<"FA terminated \n Completion begin"<<"\n";

   // try{
   //   if (bool_complete){
   // val2 = complete_clients(SI, innerTL, C,output_pattern_c);
   // cout<<"Completion terminated"<<"\n";
   // cout<<val <<" "<<val2<<"\n";
   // val = min(val,val2);
//  }
 // }

 // catch(...){
 //   bool_complete =false;
 //   cout <<"COMPLETION ERROR !!\n";
 // }
    valmin = min(valmin,val);
    if (k%5 == 4){
      ref = ref+delta;
      cout<<"ref :"<<ref<<"\n";
    }
    end = high_resolution_clock::now();
  }

  auto end2 = high_resolution_clock::now();
  float time_elapsed1 = (duration_cast<milliseconds>(end1-start)).count();
  float time_elapsed2 = (duration_cast<milliseconds>(end2-start)).count();

  //Regularization
  ofstream output_file;
  output_file.open (fname,std::ios_base::app);
  output_file<<val_init<<";"<<time_elapsed1/1000<<";"<<valmin<<";"<<time_elapsed2/1000<<"\n";
  output_file.close();


}
