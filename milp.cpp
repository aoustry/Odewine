#include <iostream>
#include <fstream>
#include "SceneInstance.cpp"
#include <cstdlib>
#include <ilcplex/ilocplex.h>
#include "CoverCutsMaker.cpp"
using namespace std::chrono;


float solve_coherent_1C(SceneInstance SI, float TL)
{
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


	//Contraintes d'interférence

	for (int cand=0; cand<m; cand++)
	{
		for (int client=0; client<n; client++)
		{
			if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma))
			{
				model.add(y[client*m+cand] ==0);

			}
			else
			{ 	//sigma +sum{k in V_cand,k2 in V_clients :k<>jand k2<>i}(p[k2,i]*y[k2,k]) + sum{ k in V_cand :k<>j}(p[k,i]*x[k])
				IloExpr noise_client(env);
				//sigma +sum{k in V_cand,k2 in V_clients:k<>j and k2<>i}(p[k2,j]*y[k2,k]) + sum{ k in V_cand :k<>j}(p[k,j]*x[k])
				IloExpr noise_cand(env);
				noise_client += SI.sigma;
				noise_cand += SI.sigma;
				for (int k=0; k<m; k++){

					if (k!=cand){
						noise_client+= SI.p[k+n][client]*x[k+n];
						noise_cand += SI.p[k+n][cand+n]*x[k+n];
						for (int k2 =0; k2<n;k2++){
							if (k2!=client){
							noise_client+= SI.p[k2][client]*y[k2*m+k];
							noise_cand+= SI.p[k2][cand+n]*y[k2*m+k];
							}
						}


					}
				}
			model.add(IloIfThen(env,(y[client*m+cand]==1), noise_client <= SI.p[cand+n][client]/(pow(2,SI.d[client]) -1)));
			model.add(IloIfThen(env,(y[client*m+cand]==1), noise_cand <=  SI.p[client][cand+n]/(pow(2,SI.u[client]) -1 )));
			noise_client.end();
			noise_cand.end();


			}
		}


	}


	IloCplex cplex(model);

	// Resolution
  cplex.setParam(IloCplex::Param::TimeLimit,TL);
   cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);


	cplex.solve();
  cout<<cplex.getObjValue()<<"\n";
  cout<<cplex.getBestObjValue()<<"\n";
	float res = cplex.getObjValue();
	x.end();
	y.end();
	env.end();


  return res ;



}

float * solve_coherent_no_noise(SceneInstance SI, float TL)
{
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
	cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);



	cplex.solve();
  cout<<cplex.getObjValue()<<"\n";
  cout<<cplex.getBestObjValue()<<"\n";
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

float * solve_coherent_MC(SceneInstance SI, float TL, int C)
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
	float bigM [n+m];
        for (int k=0; k<m+n; k++)
	{       bigM[k] = SI.sigma;
		for (int l=0; l<m+n; l++)
		{ bigM[k] = bigM[k]+SI.p[l][k] ;

		}
	}

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
        //Autre possibilité
        // for (int k=0; k<m; k++){
        //
				// 	if (k!=cand){
				// 		noise_client+= SI.p[k+n][client]*xc[k+n+color*(n+m)];
				// 		noise_cand += SI.p[k+n][cand+n]*xc[k+n+color*(n+m)];
        //   }
        // }
				// 		for (int k2 =0; k2<n;k2++){
				// 			if (k2!=client){
				// 			noise_client+= SI.p[k2][client]*(xc[k2+color*(n+m)]-yc[k2*m+cand+color*(n*m)]);
				// 			noise_cand+= SI.p[k2][cand+n]*(xc[k2+color*(n+m)]-yc[k2*m+cand+color*(n*m)]);
				// 			}
				// 		}

			//model.add(IloIfThen(env,(yc[client*m+cand+color*(n*m)]==1), noise_client <= SI.p[cand+n][client]/(pow(2,SI.d[client]) -1)));
			//model.add(IloIfThen(env,(yc[client*m+cand+color*(n*m)]==1), noise_cand <=  SI.p[client][cand+n]/(pow(2,SI.u[client]) -1 )));
			model.add((yc[client*m+cand+color*(n*m)]-1)*bigM[cand+n] + noise_client <= SI.p[cand+n][client]/(pow(2,SI.d[client]) -1));
			model.add((yc[client*m+cand+color*(n*m)]-1)*bigM[client]  + noise_cand <=  SI.p[client][cand+n]/(pow(2,SI.u[client]) -1 ));

			noise_client.end();
			noise_cand.end();


			}
		}

  }
	}


	IloCplex cplex(model);

	// Resolution
  cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::TimeLimit,TL);
  cplex.setParam(IloCplex::Param::Threads,4);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,160);
static float result[3];
try{
	cplex.solve();
	
  cout<<cplex.getObjValue()<<"\n";
  cout<<cplex.getBestObjValue()<<"\n";
	x.end();
  xc.end();
	yc.end();
  auto end = high_resolution_clock::now();
  float time_elapsed = (duration_cast<milliseconds>(end-start)).count();
  
  result[0] = cplex.getBestObjValue();
  result[1] = cplex.getObjValue() ;
  result[2] = time_elapsed/1000 ;
	env.end();

}

catch(...){
result[0] = -1;
  result[1] = -1 ;
  result[2] = -1 ;
x.end();
  xc.end();
	yc.end();
env.end();



}

    return result ;



}

float solve_coherent_1C_partial_noise(SceneInstance SI)
{

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

  //3-interference
  for (int client=0; client<n; client++)
	{	IloExpr sum_client(env);
		for (int cand=0; cand<m; cand++)
      if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma)){
        model.add(y[client*m+cand] ==0);
      }
      else
      {
        float OffD = SI.p[cand+n][client]/(pow(2,SI.d[client]) -1);
        float OffU = SI.p[client][cand+n]/(pow(2,SI.u[client]) -1);


        //Boucle sur les autres clients
        for (int k = 0 ; k<n ; k++){
          if (k!=client){
            if ((SI.sigma + SI.p[k][client]> OffD)||
                (SI.sigma + SI.p[k][cand+n]>OffU)){
                    model.add(y[client*m+cand] + x[k] <= 1 + y[k*m+cand]);
                }
          }
      }
      //Boucle sur les autres AP
      for (int k = 0 ; k<m ; k++){
        if (k!=cand){
          if ((SI.sigma + SI.p[k+n][client]> OffD)||
              (SI.sigma + SI.p[k+n][cand+n]> OffU)){
                  model.add(y[client*m+cand] + x[k+n] <= 1);
              }
        }
      }

  //     //Boucles 2 devices
  //     for (int k = 0 ; k<n ; k++){
  //       if (k!=client){
  //         for (int l = k+1; l <n; l++){
  //           if (l!=client){
  //             if ((SI.sigma + SI.p[k][client]+ SI.p[l][client]> OffD)||
  //                 (SI.sigma + SI.p[k][cand+n]+ SI.p[l][cand+n]> OffU)){
  //                   model.add(y[client*m+cand] + x[k] + x[l] <= 2 + y[k*m+cand] + y[l*m+cand]);
  //                 }
  //           }
  //         }
  //         for (int l = 0 ; l <m ;l++){
  //           if (l!=cand){
  //             if ((SI.sigma + SI.p[k][client]+ SI.p[l+n][client]> OffD)||
  //                 (SI.sigma + SI.p[k][cand+n]+ SI.p[l+n][cand+n]> OffU)){
  //                   model.add(y[client*m+cand] + x[k] + x[l+n] <= 2 + y[k*m+cand]);
  //                 }
  //           }
  //         }
  //
  //       }
  //
  //     }
  //
  //     for (int k = 0 ; k<m ; k++){
  //       if (k!=cand){
  //         for (int l = k+1; l <m; l++){
  //           if (l!=cand){
  //             if ((SI.sigma + SI.p[k+n][client]+ SI.p[l+n][client]> OffD)||
  //                 (SI.sigma + SI.p[k+n][cand+n]+ SI.p[l+n][cand+n]> OffU)){
  //                   model.add(y[client*m+cand] + x[k+n] + x[l+n] <= 2);
  //                 }
  //
  //           }
  //       }
  //     }
  // }
}
}



	IloCplex cplex(model);

	cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);



	// Resolution
	cplex.solve();
	float res = cplex.getObjValue();
	x.end();
	y.end();
	env.end();


    return res ;



}

float solve_coherent_1C_partial_noise2(SceneInstance SI, int l)
// l est le nombre de contraintes ajouté pour chaque (i,j,sens)
{
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

  //3-interference
  CoverCutsMaker CM_true;
  CoverCutsMaker CM_false;
  for (int client=0; client<n; client++)
	{
		for (int cand=0; cand<m; cand++){
      //Down
      CM_true = CoverCutsMaker(SI, client, cand, true);
      if (CM_true.kmin==0){
        model.add(y[client*m+cand] ==0);
      }
      else{

        for (int k = CM_true.kmin; k<min(SI.n+SI.m-3,CM_true.kmin +l);k++){
          CM_true.add_cut_coherent(env,model,x,y,k);
        }

      }
      CM_false = CoverCutsMaker(SI, client, cand, false);
      if (CM_false.kmin==0){
        model.add(y[client*m+cand] ==0);
      }
      else{

        for (int k = CM_false.kmin; k<min(SI.n+SI.m-3,CM_false.kmin +l);k++){
          CM_false.add_cut_coherent(env,model,x,y,k);
        }

      }
    }
}

cout<<"Done\n";



	IloCplex cplex(model);
	cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);


	// Resolution
	cplex.solve();
	float res = cplex.getObjValue();
	x.end();
	y.end();
	env.end();


    return res ;



}

float solve_full_1C(SceneInstance SI)
{

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


	//Contraintes d'interférence

	for (int cand=0; cand<m; cand++)
	{
		for (int client=0; client<n; client++)
		{
			if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma))
			{
				model.add(y[client*m+cand] ==0);

			}
			else
			{
				IloExpr noise_client(env);
				IloExpr noise_cand(env);
				noise_client += SI.sigma;
				noise_cand += SI.sigma;
				for (int k=0; k<m+n; k++){

					if ((k!=cand+n) && (k!=client)){
						noise_client+= SI.p[k][client]*x[k];
						noise_cand +=  SI.p[k][cand+n]*x[k];


					}
				}
			model.add(IloIfThen(env,(y[client*m+cand]==1), noise_client <= SI.p[cand+n][client]/(pow(2,SI.d[client]) -1)));
			model.add(IloIfThen(env,(y[client*m+cand]==1), noise_cand <=  SI.p[client][cand+n]/(pow(2,SI.u[client]) -1 )));
			noise_client.end();
			noise_cand.end();


			}
		}


	}


	IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);


	clock_t begin;
	clock_t end;
	// Resolution
	cplex.solve();
	float res = cplex.getObjValue();
	x.end();
	y.end();
	env.end();



    return res ;



}




float solve_SP_coherent_1C(SceneInstance SI,IloNumArray lstar, IloNum mu, IloBoolArray patt, IloNumArray cp, float & LBRC, vector<tuple<int, int> > &edges, float TL, float GAP){

    	IloInt n = SI.n;
    	IloInt m = SI.m;

	IloEnv env = IloEnv();
  assert(edges.size()==0);
	//Variables

	IloBoolVarArray x(env, n+m);
	IloBoolVarArray y(env, n*m);


	//Modele
	IloModel model(env);

	//Objectif
	IloExpr obj_expr(env);
	for (int client=0; client<n; client++)
	{
		obj_expr += (lstar[client] - SI.lambda*(SI.d[client]+SI.u[client])) * x[client];
	}
	for (int cand=0; cand<m; cand++)
	{
		obj_expr += (lstar[cand+n] + SI.c[cand])*x[cand+n];
	}
	obj_expr+= mu ;
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


	//Contraintes d'interférence

	for (int cand=0; cand<m; cand++)
	{
		for (int client=0; client<n; client++)
		{
			if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma))
			{
				model.add(y[client*m+cand] ==0);

			}
			else
			{ 	//sigma +sum{k in V_cand,k2 in V_clients :k<>jand k2<>i}(p[k2,i]*y[k2,k]) + sum{ k in V_cand :k<>j}(p[k,i]*x[k])
				IloExpr noise_client(env);
				//sigma +sum{k in V_cand,k2 in V_clients:k<>j and k2<>i}(p[k2,j]*y[k2,k]) + sum{ k in V_cand :k<>j}(p[k,j]*x[k])
				IloExpr noise_cand(env);
				noise_client += SI.sigma;
				noise_cand += SI.sigma;
				for (int k=0; k<m; k++){

					if (k!=cand){
						noise_client+= SI.p[k+n][client]*x[k+n];
						noise_cand += SI.p[k+n][cand+n]*x[k+n];
						for (int k2 =0; k2<n;k2++){
							if (k2!=client){
							noise_client+= SI.p[k2][client]*y[k2*m+k];
							noise_cand+= SI.p[k2][cand+n]*y[k2*m+k];
							}
						}


					}
				}
			model.add(IloIfThen(env,(y[client*m+cand]==1), noise_client <= SI.p[cand+n][client]/(pow(2,SI.d[client]) -1)));
			model.add(IloIfThen(env,(y[client*m+cand]==1), noise_cand <=  SI.p[client][cand+n]/(pow(2,SI.u[client]) -1 )));
			noise_client.end();
			noise_cand.end();

			}
		}


	}

	IloCplex cplex(model);

  //cplex.setParam(IloCplex::CutUp, 0);  // 0 car mu est déjà inclus dedans !
  cplex.setParam(IloCplex::Param::TimeLimit,TL);
  cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap,GAP);
  cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::Threads,8);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);

	// Resolution
	cplex.setOut(env.getNullStream());
	cplex.solve();
  IloNumArray pattern_aux (env) ;
	cplex.getValues(x, pattern_aux);
  for (int v = 0; v<n+m ; v++)
    patt[v]=round(pattern_aux[v]);

	cp[0] = 0;
	for (int client=0; client<n; client++)
	{
		cp[0] += - SI.lambda*(SI.d[client]+SI.u[client]) * patt[client];
	}
	for (int cand=0; cand<m; cand++)
	{
		cp[0] += SI.c[cand]*patt[cand+n];
	}

  for (int client=0; client<n; client++)
    for (int cand = 0; cand <m; cand++)
      if (cplex.getValue(y[client*m+cand])>0.001)
        edges.push_back(make_tuple(client,cand));

	float reduced_cost = cplex.getObjValue();

	cout<<"Reduced cost : " << reduced_cost << "\n";
  LBRC = cplex.getBestObjValue();
  cout<<"LB Best reduced cost : " << LBRC << "\n";

	x.end();
	y.end();
	env.end();
 return reduced_cost<-0.001;


}

float solve_SP_coherent_1C_partial_noise(SceneInstance SI,IloNumArray lstar, IloNum mu){

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
		obj_expr += (lstar[client] - SI.lambda*(SI.d[client]+SI.u[client])) * x[client];
	}
	for (int cand=0; cand<m; cand++)
	{
		obj_expr += (lstar[cand+n] + SI.c[cand])*x[cand+n];
	}
	obj_expr+= mu ;
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


	//Contraintes d'interférence

  for (int client=0; client<n; client++)
	{	IloExpr sum_client(env);
		for (int cand=0; cand<m; cand++)
      if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma)){
        model.add(y[client*m+cand] ==0);
      }
      else
      {
        float OffD = SI.p[cand+n][client]/(pow(2,SI.d[client]) -1);
        float OffU = SI.p[client][cand+n]/(pow(2,SI.u[client]) -1);


        //Boucle sur les autres clients
        for (int k = 0 ; k<n ; k++){
          if (k!=client){
            if ((SI.sigma + SI.p[k][client]> OffD)||
                (SI.sigma + SI.p[k][cand+n]>OffU)){
                    model.add(y[client*m+cand] + x[k] <= 1 + y[k*m+cand]);
                }
          }
      }
      //Boucle sur les autres AP
      for (int k = 0 ; k<m ; k++){
        if (k!=cand){
          if ((SI.sigma + SI.p[k+n][client]> OffD)||
              (SI.sigma + SI.p[k+n][cand+n]> OffU)){
                  model.add(y[client*m+cand] + x[k+n] <= 1);
              }
        }
      }
		}


	}

	IloCplex cplex(model);

  cplex.setParam(IloCplex::CutUp, 0);  // 0 car mu est déjà inclus dedans !
 cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);



	// Resolution
	cplex.setOut(env.getNullStream());
	cplex.solve();


	float reduced_cost = cplex.getObjValue();

	cout<<"LB Reduced cost : " << reduced_cost << "\n";

	x.end();
	y.end();
	env.end();


     return reduced_cost<-0.0001;


}


float solve_SP_coherent_1C_partial_noise2(SceneInstance SI,IloNumArray lstar, IloNum mu,int l){

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
		obj_expr += (lstar[client] - SI.lambda*(SI.d[client]+SI.u[client])) * x[client];
	}
	for (int cand=0; cand<m; cand++)
	{
		obj_expr += (lstar[cand+n] + SI.c[cand])*x[cand+n];
	}
	obj_expr+= mu ;
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


	//Contraintes d'interférence

  //3-interference
  CoverCutsMaker CM_true;
  CoverCutsMaker CM_false;
  for (int client=0; client<n; client++)
	 {
		for (int cand=0; cand<m; cand++){
      //Down
      CM_true = CoverCutsMaker(SI, client, cand, true);
      if (CM_true.kmin==0){
        model.add(y[client*m+cand] ==0);
      }
      else{

        for (int k = CM_true.kmin; k<min(SI.n+SI.m-3,CM_true.kmin +l);k++){
          CM_true.add_cut_coherent(env,model,x,y,k);
        }

      }
      CM_false = CoverCutsMaker(SI, client, cand, false);
      if (CM_false.kmin==0){
        model.add(y[client*m+cand] ==0);
      }
      else{

        for (int k = CM_false.kmin; k<min(SI.n+SI.m-3,CM_false.kmin +l);k++){
          CM_false.add_cut_coherent(env,model,x,y,k);
        }

      }
    }
  }
	IloCplex cplex(model);

  cplex.setParam(IloCplex::CutUp, 0);  // 0 car mu est déjà inclus dedans !

	// Resolution
	cplex.setOut(env.getNullStream());
       cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);

	cplex.solve();


	float reduced_cost = cplex.getObjValue();

	cout<<"LB Reduced cost : " << reduced_cost << "\n";

	x.end();
	y.end();
	env.end();


     return reduced_cost<-0.0001;


}


float coherent_from_pattern(SceneInstance SI, vector<vector<int>> plist, int * cplist, int pnumber, int C, int * final_solution){

  IloInt n = SI.n;
  IloInt m = SI.m;
  IloEnv env = IloEnv();

  //Variables

  IloBoolVarArray z(env, pnumber);

  //Modele
  IloModel model(env);

  //Objectif
  IloExpr obj_expr(env);
  for (int p=0; p<pnumber; p++){
    obj_expr+=cplist[p]*z[p];
  }
  model.add(IloMinimize(env, obj_expr));
  obj_expr.end();

  //contraintes
  model.add(IloSum(z)<=C);

  vector <IloExpr> sum_devices = {};
  for (int i = 0 ; i<n+m ; i++){
		IloExpr e (env);
		sum_devices.push_back(e);
	}
  int i;
  for (int p = 0; p<pnumber; p++){
    int l = plist[p].size();
    for (int idx = 0; idx <l ; idx++){
      i = plist[p][idx];
      sum_devices[i]+=z[p];
    }
  }
  for (int i = 0 ; i<n+m ; i++){
		model.add(sum_devices[i]<=1);
	}


	// Resolution
  IloCplex cplex(model);
	//cplex.setOut(env.getNullStream());
  cplex.setParam(IloCplex::Param::TimeLimit,150);
  cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);


	cplex.solve();

  for (int i = 0; i<n+m;i++)
    final_solution[i] = cplex.getValue(sum_devices[i]);

	float res = cplex.getObjValue();
	env.end();

  return res;


}

float solve_coherent_MC_local(SceneInstance SI, float TL, int C, int * xstar, int k)
{
  auto start = high_resolution_clock::now();
  IloInt n = SI.n;
  IloInt m = SI.m;

	IloEnv env = IloEnv();

	//Variables
  IloBoolVarArray delta(env, m);
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
  model.add(IloSum(delta)<=k);
  for (int i = 0; i<m;i++){
    model.add(x[i+n]-xstar[i+n]<=delta[i]);
    model.add(xstar[i+n]-x[i+n]<=delta[i]);
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
  cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);


	cplex.solve();
  cout<<cplex.getObjValue()<<"\n";
  cout<<cplex.getBestObjValue()<<"\n";
	x.end();
  	xc.end();
	yc.end();
  	auto end = high_resolution_clock::now();
  	float time_elapsed = (duration_cast<milliseconds>(end-start)).count();
	float res = cplex.getObjValue() ;
	env.end();

  return res;


}

float * feas_coherent_MC(SceneInstance SI, float TL, int C, float value)
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
        model.add(obj_expr == value);
	obj_expr.end();

	// Contraintes

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
cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);
	cplex.solve();
  cout<<cplex.getObjValue()<<"\n";
  cout<<cplex.getBestObjValue()<<"\n";
	x.end();
  xc.end();
	yc.end();
  auto end = high_resolution_clock::now();
  float time_elapsed = (duration_cast<milliseconds>(end-start)).count();
  static float result[3];
  result[0] = cplex.getBestObjValue();
  result[1] = cplex.getObjValue() ;
  result[2] = time_elapsed/1000 ;
	env.end();


    return result ;



}

float solve_SP_coherent_from_pattern(SceneInstance SI,IloNumArray xref, IloNumArray lstar, IloNum mu, IloBoolArray patt, IloNumArray cp,vector<tuple<int, int> > &edges, float TL, float GAP){

    	IloInt n = SI.n;
    	IloInt m = SI.m;

	IloEnv env = IloEnv();
  assert(edges.size()==0);
	//Variables

	IloBoolVarArray x(env, n+m);
	IloBoolVarArray y(env, n*m);


	//Modele
	IloModel model(env);

	//Objectif
	IloExpr obj_expr(env);
	for (int client=0; client<n; client++)
	{
		obj_expr += (lstar[client] - SI.lambda*(SI.d[client]+SI.u[client])) * x[client];
	}
	for (int cand=0; cand<m; cand++)
	{
		obj_expr += (lstar[cand+n] + SI.c[cand])*x[cand+n];
	}
	obj_expr+= mu ;
	model.add(IloMinimize(env, obj_expr));
	obj_expr.end();

	// Contraintes

  //0- Same-AP as xref
  for (int cand = 0; cand<m;cand++)
    model.add(x[n+cand]==xref[n+cand]);

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


	//Contraintes d'interférence

	for (int cand=0; cand<m; cand++)
	{
		for (int client=0; client<n; client++)
		{
			if ((SI.p[cand+n][client]/(pow(2,SI.d[client]) -1) < SI.sigma) || (SI.p[client][cand+n]/(pow(2,SI.u[client]) -1) < SI.sigma))
			{
				model.add(y[client*m+cand] ==0);

			}
			else
			{ 	//sigma +sum{k in V_cand,k2 in V_clients :k<>jand k2<>i}(p[k2,i]*y[k2,k]) + sum{ k in V_cand :k<>j}(p[k,i]*x[k])
				IloExpr noise_client(env);
				//sigma +sum{k in V_cand,k2 in V_clients:k<>j and k2<>i}(p[k2,j]*y[k2,k]) + sum{ k in V_cand :k<>j}(p[k,j]*x[k])
				IloExpr noise_cand(env);
				noise_client += SI.sigma;
				noise_cand += SI.sigma;
				for (int k=0; k<m; k++){

					if (k!=cand){
						noise_client+= SI.p[k+n][client]*x[k+n];
						noise_cand += SI.p[k+n][cand+n]*x[k+n];
						for (int k2 =0; k2<n;k2++){
							if (k2!=client){
							noise_client+= SI.p[k2][client]*y[k2*m+k];
							noise_cand+= SI.p[k2][cand+n]*y[k2*m+k];
							}
						}


					}
				}
			model.add(IloIfThen(env,(y[client*m+cand]==1), noise_client <= SI.p[cand+n][client]/(pow(2,SI.d[client]) -1)));
			model.add(IloIfThen(env,(y[client*m+cand]==1), noise_cand <=  SI.p[client][cand+n]/(pow(2,SI.u[client]) -1 )));
			noise_client.end();
			noise_cand.end();

			}
		}


	}

	IloCplex cplex(model);

  //cplex.setParam(IloCplex::CutUp, 0);  // 0 car mu est déjà inclus dedans !
  cplex.setParam(IloCplex::Param::TimeLimit,TL);
  cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap,GAP);
  cplex.setParam(IloCplex::Param::Emphasis::Memory,1);
  cplex.setParam(IloCplex::Param::Threads,8);
  cplex.setParam(IloCplex::Param::MIP::Strategy::File,2);
  cplex.setParam(IloCplex::Param::WorkMem,8000);

	// Resolution
	cplex.setOut(env.getNullStream());
	cplex.solve();
  IloNumArray pattern_aux (env) ;
	cplex.getValues(x, pattern_aux);
  for (int v = 0; v<n+m ; v++)
    patt[v]=0;


  for (int client=0; client<n; client++)
    for (int cand = 0; cand <m; cand++)
      if (cplex.getValue(y[client*m+cand])>0.001){
        edges.push_back(make_tuple(client,cand));
        patt[client]=1;
        patt[cand+n]=1;
      }
      cp[0] = 0;
    	for (int client=0; client<n; client++)
    	{
    		cp[0] += - SI.lambda*(SI.d[client]+SI.u[client]) * patt[client];
    	}
    	for (int cand=0; cand<m; cand++)
    	{
    		cp[0] += SI.c[cand]*patt[cand+n];
    	}

	float reduced_cost = cplex.getObjValue();

	cout<<"Polished Reduced cost : " << reduced_cost << "\n";

	x.end();
	y.end();
	env.end();
 return reduced_cost<-0.001;


}
