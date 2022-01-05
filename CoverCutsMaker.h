
class CoverCutsMaker
{
public:
CoverCutsMaker();
~CoverCutsMaker();
CoverCutsMaker(SceneInstance SI_entry, int i_entry, int j_entry, bool down_entry);
void add_cut_coherent(IloEnv e, IloModel m, IloBoolVarArray x, IloBoolVarArray y, int k);
int kmin;
int kmax;

private:
  vector<int> sorted_neighbours ;
  SceneInstance SI;
  int n;
  int m;
  int i;
  int j;
  bool down;
  float threshold;

};
