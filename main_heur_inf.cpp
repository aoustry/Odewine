#include <iostream>
#include <fstream>
#include "heur.cpp"
#include <cstdlib>


ILOSTLBEGIN

#include <chrono>

using namespace std;
using namespace std::chrono;


int main(int argc, char**argv){

    const char * reg = argv[1];
    const int color_number = atoi(argv[2]);
    const float perturbation = ((float)atoi(argv[3]))/10;
    assert(perturbation>1);
    cout<<"Channels number ;"<<color_number<<"\n";
    cout <<"Perturbation : "<<perturbation<<"\n";
    
    std::string path = "output/output_heur";
    std::string lastName  = std::to_string(color_number)+"_"+std::to_string(perturbation);
    std::string extension = ".csv";
    std::string fullName = path + lastName + extension;
    
    //Output file
    ofstream output_file;
    output_file.open (fullName);
    output_file<<"Channels number ;"<<color_number<<"\n";
    output_file <<"Perturbation : "<<perturbation<<"\n";
    output_file << "Instance name ;|I| ;|J|;RH1 UB;RH1 time;RH2 UB;RH2 time\n";

    // Parameters
    const float globalTL = 3600;
    const float TL = 100;
    const float GAP = 0.1;
    IloNum C = color_number;

    std::ifstream ifs (reg, std::ifstream::in);
   	char buffer [100];
    ifs.getline(buffer,100);
    int nb_inst = atoi(buffer);
    cout<<"Instances number :"<<nb_inst <<"\n";
    output_file.close();

    for (int inst_idx =0; inst_idx<nb_inst; inst_idx++ ){

      ifs.getline(buffer,100);
      output_file.open (fullName,std::ios_base::app);
      cout << buffer<<"\n";
      SceneInstance SI = SceneInstance(buffer);
      output_file<<buffer<<";"<<SI.n<<";"<<SI.m<<";";
      output_file.close();
      //TIME LIMIT
      RH1andRH2(SI, TL, C,globalTL, perturbation,fullName);
    }

    return 0;
}
