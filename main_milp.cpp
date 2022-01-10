#include <iostream>
#include <fstream>
#include "milp.cpp"
#include <cstdlib>


ILOSTLBEGIN

#include <chrono>

using namespace std;
using namespace std::chrono;


//File for executing the numerical experiments with CPLEX

int main(int argc, char**argv){
    const char * reg = argv[1];
    const int color_number = atoi(argv[2]);
    // Parameters
    const float TL = 3600;

    IloNum C = color_number;
    
    std::string path = "output/output_milp";
    std::string lastName  = std::to_string(color_number);
    std::string extension = ".csv";
    std::string fullName = path + lastName + extension;
    
    ofstream output_file;
    output_file.open (fullName);
    output_file << "Channels number:"<<color_number<<"\n";
    output_file << "Time limit (s)"<<TL<<"\n";
    output_file << "Instance name ;|I| ;|J|; CPLEX LB; CPLEX UB; CPLEX time\n";
    std::ifstream ifs (reg, std::ifstream::in);
   	char buffer [100];
    ifs.getline(buffer,100);
    int nb_inst = atoi(buffer);
    cout<<"Instances number :"<<nb_inst <<"\n";
      float * result;
      output_file.close();

    for (int inst_idx =0; inst_idx<nb_inst; inst_idx++ ){

                 ifs.getline(buffer,100);
                 output_file.open(fullName,std::ios_base::app);
                 cout << buffer<<"\n";
                 SceneInstance SI = SceneInstance(buffer);
                 output_file<<buffer<<";"<<SI.n<<";"<<SI.m<<";";

                 //  result1 = solve_coherent_MC_agg(SI, TL, C);

                 result = solve_coherent_MC(SI, TL, C);
                 cout << buffer<<" solved \n";
	         //output_file<<result1[0]<<";"<<result1[1]<<";"<<result1[2]<<";";
                 output_file<<result[0]<<";"<<result[1]<<";"<<result[2]<<"\n";
                 output_file.close();

    }

    return 0;
}
