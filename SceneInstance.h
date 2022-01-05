

#include <vector>

using namespace std;
using Iter = std::vector<int>::const_iterator;

class SceneInstance
{
public:
	SceneInstance(){};
	SceneInstance(const char* filename);
	SceneInstance(SceneInstance SI, vector<int> clients_delete, vector<int> ap_delete);
	int n;
	int m;
	int lambda;
	int gamma ;
	double sigma;
	vector<int> c;
	vector<double> d;
	vector<double> u;
	vector < vector<double> > p;

};
