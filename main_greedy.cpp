#include <iostream>
#include <fstream>
#include "GreedyHeuristicM.cpp"
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace std::chrono;

//File for executing the numerical experiments with the 2 Greedy Heuristics

float GreedyHeuristic1(SceneInstance SI, int frequencies_number){
//Algorithm GH1
  SceneInstance aux = SceneInstance(SI,{},{});
  float result = 0 ;
  for (int i = 0; i<SI.n ;i++){
    result+= SI.lambda*(SI.d[i]+SI.u[i]);
  }
  for (int fr = 0 ; fr<frequencies_number; fr++){
    vector<tuple<int, int> > edges = {};
    GreedyHeuristicM GH = GreedyHeuristicM(aux,{});
    vector <int> AP_delete ={};
    vector <int> client_delete = {};
    result+=GH.compute(edges,client_delete,AP_delete);
    aux = SceneInstance(aux,client_delete,AP_delete);
  }

  return result;

}

float auxiliary_GreedyHeuristicMultipleChannelTabu(SceneInstance SI, int frequencies_number, vector<int> tabu, int & first_aps ){

  SceneInstance aux = SceneInstance(SI,{},{});
  float result = 0 ;
  for (int i = 0; i<SI.n ;i++){
    result+= SI.lambda*(SI.d[i]+SI.u[i]);
  }



  for (int fr = 0 ; fr<frequencies_number; fr++){
    vector<tuple<int, int> > edges = {};

	 if (fr==0){
      GreedyHeuristicM GH = GreedyHeuristicM(aux,tabu);
      vector <int> AP_delete ={};
      vector <int> client_delete = {};
      result+=GH.compute(edges,client_delete,AP_delete);
      aux = SceneInstance(aux,client_delete,AP_delete);
      if (AP_delete.size()>0) {
        first_aps = AP_delete[0];
    }
   }
   else{
     GreedyHeuristicM GH = GreedyHeuristicM(aux,{});
     vector <int> AP_delete ={};
     vector <int> client_delete = {};
     result+=GH.compute(edges,client_delete,AP_delete);
     aux = SceneInstance(aux,client_delete,AP_delete);
   }
}

  return result;

}

float GreedyHeuristic2(SceneInstance SI, int frequencies_number, int tries){
//Algorithm GH2
  vector <int> tabu = {};
	float res = 2000000;
	for (int k = 0 ; k<tries;k++){
		int first_aps ;
		float temp = auxiliary_GreedyHeuristicMultipleChannelTabu( SI,  frequencies_number, tabu, first_aps );
		res = min(res,temp);
		tabu.push_back(first_aps);
	}

	return res;

}


int main(int argc, char**argv){

    const char * list = argv[1];
	  //const int tries = atoi(argv[2]);
    const int color_number = atoi(argv[2]);
    // Parameters
    const float TL = 3600;

    ofstream output_file;
    output_file.open ("output_greedy.csv");
    output_file << "Channels number:"<<color_number<<"\n";
    output_file << "Instance name ;|I| ;|J|;GH1 UB; GH1 time;GH2 UB; GH2 time \n";
    std::ifstream ifs (list, std::ifstream::in);
   	char buffer [100];
    ifs.getline(buffer,100);
    int nb_inst = atoi(buffer);
    cout<<"Instances number :"<<nb_inst <<"\n";
    float  result1, result2;
    output_file.close();
    for (int inst_idx =0; inst_idx<nb_inst; inst_idx++ ){

      ifs.getline(buffer,100);
      output_file.open ("output_greedy.csv",std::ios_base::app);
      cout << buffer<<"\n";
      SceneInstance SI = SceneInstance(buffer);
      output_file<<buffer<<";"<<SI.n<<";"<<SI.m<<";";
      auto start = high_resolution_clock::now();
      result1 = GreedyHeuristic1(SI,color_number);
      auto end = high_resolution_clock::now();
      float time_elapsed1 = (duration_cast<milliseconds>(end-start)).count();
      start = high_resolution_clock::now();
      int tries = SI.m;
      cout <<"APnumber"<<SI.m<<"\n";
	    result2 = GreedyHeuristic2(SI,color_number,tries);
      end = high_resolution_clock::now();
      float time_elapsed2 = (duration_cast<milliseconds>(end-start)).count();
      output_file<<result1 <<";"<<time_elapsed1/1000<<";"<<result2 <<";"<<time_elapsed2/1000<<"\n";
      output_file.close();
    }
    return 0;
}
