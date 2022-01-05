#include <stdlib.h>
#include <cfloat> // for DBL_MAX
#include <cmath>  // for fabs()
#include <iostream>
#include <vector>
#include <fstream>
#include "SceneInstance.h"
#include <assert.h>

bool is_element(vector<int> vec, int el){

	for (Iter it = vec.begin() ; it<vec.end();it++){
		if (*it==el){
			return true;
		}
	}
return false;
}
SceneInstance::SceneInstance(const char* filename){


   	std::ifstream ifs (filename, std::ifstream::in);
   	char buffer [25];

   	//Lecture de n
	ifs.getline(buffer,25);
	//std::cout<<buffer<< "\n";
    	n = atoi (buffer);
	//std::cout << n << "\n";

	//Lecture de n
	ifs.getline(buffer,25);
	//std::cout<<buffer<< "\n";
    	 m = atoi(buffer);
	//std::cout << m << "\n";

	//Lecture de lambda
	ifs.getline(buffer,25);
	//std::cout<<buffer<< "\n";
    	lambda = atoi(buffer);
	//std::cout << lambda << "\n";

	//Lecture de gamma
	ifs.getline(buffer,25);
	//std::cout<<buffer<< "\n";
    	gamma = atoi(buffer);
	//std::cout << gamma << "\n";

	//Lecture de gamma
	ifs.getline(buffer,25);
	//std::cout<<buffer<< "\n";
    	sigma = stod(buffer);
	//std::cout << sigma << "\n";

	//Lecture de c
	for (int i = 0 ; i<m ;i++){
		ifs.getline(buffer,25);
		//std::cout<<buffer<< "\n";
	    	c.push_back(atoi(buffer));
		//std::cout << c[i] << "\n";

	}

	//Lecture de d
	for (int i = 0 ; i<n ;i++){
		ifs.getline(buffer,25);
		//std::cout<<buffer<< "\n";
	    	d.push_back(stod(buffer));
		//std::cout << d[i] << "\n";
	}

	//Lecture de u
	for (int i = 0 ; i<n ;i++){
		ifs.getline(buffer,25);
		//std::cout<<buffer<< "\n";
	    	u.push_back(stod(buffer));
		//std::cout << u[i] << "\n";

	}


	//Lecture de p
	for (int i = 0 ; i<n+m ;i++){
		vector<double> aux ;
		for (int j =0; j <n+m; j++){
			ifs.getline(buffer,25);
			//std::cout<<buffer<< "\n";
		    	aux.push_back(stold(buffer));
			//std::cout << aux[j] << "\n";
		}
		p.push_back(aux);

	}

}

SceneInstance::SceneInstance(SceneInstance SI,vector<int> clients_delete, vector<int> ap_delete){
  n = SI.n - clients_delete.size() ;
	m = SI.m - ap_delete.size();
	lambda = SI.lambda;
	gamma = SI.gamma ;
	sigma = SI.sigma;
  vector <int> list_indices = {};


  u = {}; d= {};
  for (int k = 0 ; k<SI.n;k++){
    if (is_element(clients_delete,k)==false){
      list_indices.push_back(k);
      u.push_back(SI.u[k]);
      d.push_back(SI.d[k]);
    }
  }
  c = {};
  for (int k = 0 ; k<SI.m;k++){
    if (is_element(ap_delete,k)==false){
      c.push_back(SI.c[k]);
      list_indices.push_back(k+SI.n);
    }
  }
  assert(list_indices.size()==n+m);
	p = {};
  for (int ind1 = 0 ; ind1<list_indices.size();ind1++ ){
    vector <double> line = {};
    for (int ind2 = 0 ; ind2<list_indices.size();ind2++ ){
      line.push_back(SI.p[list_indices[ind1]][list_indices[ind2]]);
    }
    p.push_back(line);
  }

}
