#include <vector>
#include <fstream>
#include <bits/stdc++.h>
#include "GreedyHeuristicM.h"
#include <assert.h>

using Iter = std::vector<int>::const_iterator;
using namespace std;

vector<int> erase_element(vector<int> vec, int el){
	vector<int> result;
	for (Iter it = vec.begin() ; it<vec.end();it++){
		if (*it!=el){
			result.push_back(*it);

		}
	}

return result;
}


GreedyHeuristicM::GreedyHeuristicM(SceneInstance SI_input, vector<int> tabu){

	SI = SI_input;
	n = SI.n;
	m = SI.m;
	gamma_self = SI.gamma;
	tabu_ap = tabu;

	for (int i = 0; i<n ;i++){
		objective.push_back(- SI.lambda*(SI.d[i]+SI.u[i]));
	}


	for (int j =0 ; j<m; j++){
		objective.push_back(SI.c[j]);
		ap_pot.push_back(j);
		y_on.push_back({});

	}

	for (int i = 0; i<n ;i++){
		vector <float> lineD ={};
		vector <float> lineU ={};
		for (int j =0 ; j<m; j++){
			lineD.push_back((SI.p[j+n][i]/(pow(2,SI.d[i]) -1)) - SI.sigma);
			lineU.push_back((SI.p[i][j+n]/(pow(2,SI.u[i]) -1)) - SI.sigma);
		}
			MarginD.push_back(lineD);
			MarginU.push_back(lineU);
	}

	// On trie les clients
	vector<tuple<float, int> > v;
	for (int i = 0; i<n; i++){
		v.push_back(make_tuple(objective[i],i));
	}
	// Using sort() function to sort by 1st element of tuple
	random_shuffle ( v.begin(), v.end() );
  sort(v.begin(), v.end());
	int i;
	tuple<float, int> t;
	for (int k = 0; k<n; k++){
		  t = v[k];
			i = get<1>(t);
			cl_pot.push_back(i);

		}

	//Brassage aléatoire des AP
	random_shuffle ( ap_pot.begin(), ap_pot.end() );
}


GreedyHeuristicM::~GreedyHeuristicM(){};
vector<int> GreedyHeuristicM::selection(vector<int> tabu_iter){

	float remaining_capa [m];

	for (int j =0 ; j<m; j++){
		remaining_capa[j] = gamma_self;
	}

	float score [m];
	vector <int> clients_r [m];
	int i;
	int j;

	//Init score
	for (Iter it2 = ap_pot.begin(); it2!=ap_pot.end(); ++it2){
		j = *it2;
		score[j] = objective[j+n];

	}

	int ap ;
	int client;
	int compteur_client;

	//Evaluation des score
	for (Iter it2 = ap_pot.begin(); it2!=ap_pot.end(); ++it2){
		j = *it2; if (is_element(tabu_iter,j)==false){
	 	 Matrix MU_group = initM(MarginU,j,false);
		 Matrix MD_group = initM(MarginD,j,true);
		for (Iter it1 = cl_pot.begin(); it1!=cl_pot.end(); ++it1){
			i = *it1;
			if (remaining_capa[j]>=(SI.d[i]+SI.u[i])){ // SI reste de la capacité
				if ((MarginD[i][j]>=0) && (MarginU[i][j])>=0){// SI communication possible

					if (check_possible(i, false, MU_group, MD_group)){
						remaining_capa[j] = remaining_capa[j]-(SI.d[i]+SI.u[i]);
						clients_r[j].push_back(i);
						score[j]+=objective[i];

						//Update MU_group et MD_group
						for (Iter it3 = ap_on.begin(); it3!= ap_on.end(); it3++){

							ap = *it3;
							vector<int> connected_clients = y_on[ap];
							compteur_client = 0;
							for (Iter it4 = connected_clients.begin(); it4!= connected_clients.end(); it4++){
								client = *it4;
								MU_group[ap][compteur_client]+=-SI.p[i][ap+n];
								MD_group[ap][compteur_client]+=-SI.p[i][client];
								assert(MU_group[ap][compteur_client]>=0);
								assert(MD_group[ap][compteur_client]>=0);
								compteur_client++;
							}
						}
					}
			}
		}
	}
	}
}
	//Sélection du meilleur AP
	int new_ap = ap_pot[0]; //Marche pas si AP pot est vide !!
	for (Iter it2 = ap_pot.begin(); it2!=ap_pot.end(); ++it2){
		j = *it2;

		if (score[new_ap]>score[j]){
			new_ap = j;
		}
	}
	vector <int> result;
	if (score[new_ap]<0){
		result = clients_r[new_ap];
		result.insert(result.begin(), new_ap);
		// Le numéro de l'AP est inséré en première position
	}

	return result;
}

void GreedyHeuristicM::update(vector <int> selected_ones){

	int new_ap = selected_ones[0];

	//MAJ AP
	ap_on.push_back(new_ap);
	int p = ap_pot.size();
	ap_pot = erase_element(ap_pot,new_ap);
	assert(ap_pot.size() == p-1);

	//MAJ CL and Y
	int i;

	for (Iter it = selected_ones.begin()+1; it< selected_ones.end(); it++){
		i = *it;
		cl_on.push_back(i);
		y_on[new_ap].push_back(i);
		int p = cl_pot.size();
		cl_pot = erase_element(cl_pot,i);
		assert(cl_pot.size() == p-1);

	}

	//cout << "Size connected clients :" << y_on[new_ap].size() <<"\n";


	//MAJ Margin pour arêtes potentielles
	int j;
	for (Iter it = cl_pot.begin(); it!= cl_pot.end(); it++){
		i = *it;
		for (Iter it2 = ap_pot.begin(); it2!= ap_pot.end(); it2++){
			j = *it2;
			MarginU[i][j]+= -SI.p[new_ap+n][j+n];
			MarginD[i][j]+= -SI.p[new_ap+n][i];

			for (Iter it3 = selected_ones.begin()+1; it3!= selected_ones.end(); it3++){
				int i_noise = *it3;
				MarginU[i][j]+= -SI.p[i_noise][j+n];
				MarginD[i][j]+= -SI.p[i_noise][i];
			}
		}
	}

	//MAJ Margin pour arêtes existantes
	for (Iter it2 = ap_on.begin(); it2!= ap_on.end(); it2++){
		j = *it2;
		if (j!=new_ap){ //On ne doit pas compter le bruit provenant des toutes nouvelles connexions
			vector<int> connected_clients = y_on[j];
			for (Iter it = connected_clients.begin(); it!= connected_clients.end(); it++){
				i = *it;
				MarginU[i][j]+= -SI.p[new_ap+n][j+n];
				MarginD[i][j]+= -SI.p[new_ap+n][i];
				assert(MarginD[i][j]>=0);
				assert(MarginU[i][j]>=0);

				for (Iter it3 = selected_ones.begin()+1; it3!= selected_ones.end(); it3++){
					int i_noise = *it3;
					MarginU[i][j]+= -SI.p[i_noise][j+n];
					MarginD[i][j]+= -SI.p[i_noise][i];
					assert(MarginU[i][j]>=0);
					assert(MarginD[i][j]>=0);
				}
			}

		}
	}


}

void GreedyHeuristicM::clean(){

	int i;
	int j;
	vector<int> ap_to_erase ;
	vector<int> clients_to_erase;
	Matrix MU = initM(MarginU,-1,false);
	Matrix MD = initM(MarginD,-1,true);

	for (Iter it = cl_pot.begin(); it!= cl_pot.end(); it++){
		i = *it;
		if (check_possible(i,false,MU,MD) == false){
			clients_to_erase.push_back(*it);
		}
	}
	for (Iter it = ap_pot.begin(); it!= ap_pot.end(); it++){
		j = *it;
		if (check_possible(j,true,MU,MD) == false){
			ap_to_erase.push_back(*it);
		}
	}
	for (int idx = 0; idx<ap_to_erase.size(); idx++){
		ap_pot = erase_element(ap_pot,ap_to_erase[idx]);

	}
	for (int idx = 0; idx<clients_to_erase.size(); idx++){
		cl_pot = erase_element(cl_pot,clients_to_erase[idx]);

	}

}

bool GreedyHeuristicM::check_possible(int k, bool is_ap, Matrix MU , Matrix MD ){

	int device = k;
	int i ;
	int j;
	int compteur;
	if (is_ap){
		device+=n;
	}
	//Boucles sur les arêtes existantes
	for (Iter it2 = ap_on.begin(); it2!= ap_on.end(); it2++){
		j = *it2;
		vector<int> connected_clients = y_on[j];
		compteur = 0;
		for (Iter it = connected_clients.begin(); it!= connected_clients.end(); it++){
			i = *it;
			if ((MU[j][compteur] < SI.p[device][j+n]) || (MD[j][compteur] <SI.p[device][i])){
				return false;
			}
			compteur++;
		}
	}
	return true;
}

vector <vector <float> > GreedyHeuristicM::initM(Matrix B, int AP, bool down){
	vector <vector <float> > M ;
	for (int i = 0 ; i<m;i++){
		M.push_back({});
	}
	int i;
	int j;
	for (Iter it2 = ap_on.begin(); it2!= ap_on.end(); it2++){
		j = *it2;
		vector<int> connected_clients = y_on[j];

		for (Iter it = connected_clients.begin(); it!= connected_clients.end(); it++){
			i = *it;

			if (AP>=0){
				if (down){
					(M[j]).push_back(B[i][j] - SI.p[AP+n][i]);
				}
				else{
					(M[j]).push_back(B[i][j] - SI.p[AP+n][j+n]);
				}
			}
			else {

					(M[j]).push_back(B[i][j]);

			}
		}
	}
	return M;
}

float GreedyHeuristicM::compute(vector<tuple<int, int> > &edges,vector< int > &client,vector< int > &AP){

	assert(edges.size()==0);
	assert(AP.size()==0);
	assert(client.size()==0);
	///Algorithm
	vector<int> SO;
	bool running = true ;
	vector<int> tabu_iter = tabu_ap ;


		while (running){
			SO = selection(tabu_iter);
			// A partir de la deuxième sélection, plus de tabou
			tabu_iter = {} ;
			if (SO.size()>=2){
				update(SO);
				clean();
				running = (ap_pot.size()>0);
			}
			else{
				running = false;
			}

		}


	///Exporting results

		float result_without_offset = 0;
		int i ; int j;
		for (Iter it2 = ap_on.begin(); it2!= ap_on.end(); it2++){
			j = *it2;
			result_without_offset+= SI.c[j];
			vector<int> connected_clients = y_on[j];
			AP.push_back(j);
			for (Iter it = connected_clients.begin(); it!= connected_clients.end(); it++){
				i = *it;
				result_without_offset+= - SI.lambda*(SI.d[i]+SI.u[i]);
				client.push_back(i);
				edges.push_back(make_tuple(i,j));

			}
		}

		return result_without_offset;

	// Si ap_on est vide, pattern est un tableau de 0 et cp[0] vaut 0 : ok
}


bool GreedyHeuristicM::connected(int client, int ap){
	int i;
	vector<int> connected_clients = y_on[ap];

	for (Iter it = connected_clients.begin(); it!= connected_clients.end(); it++){
		i = *it;
		if (i==client){return true;}
	}
	return false;
}
