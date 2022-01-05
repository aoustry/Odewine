#include <vector>
#include "SceneInstance.cpp"
using namespace std;
using Matrix = vector < vector<float> >;

class GreedyHeuristicM
{
public:
	float gamma_self;
	GreedyHeuristicM();
	GreedyHeuristicM(SceneInstance SI_input,vector<int> tabu);

	~GreedyHeuristicM();
	float compute(vector<tuple<int, int> > &edges,vector< int > &client,vector< int > &AP);

	vector<int> ap_on;
private:
	SceneInstance SI;
	int n;
	int m;
	int frequencies_number;
	vector <float> objective;
	vector<int> tabu_ap;
	vector<int> cl_on;
	vector<int> cl_pot;
	vector<int> ap_pot;
	vector < vector<int> > y_on ;

	Matrix MarginU;
	Matrix MarginD;


	vector<int> selection(vector<int> tabu_iter);
	void update(vector<int> selected_ones);
		void clean();
	vector <vector <float> > initM(Matrix B, int AP, bool down);
	bool check_possible(int k, bool is_ap, Matrix MU , Matrix MD );
	bool connected(int client, int ap);



};
