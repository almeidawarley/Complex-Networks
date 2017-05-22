// GETComp.cpp : Defines the entry point for the console application.
//
#include "graph.h"
#include "stdafx.h"
#include <ilcplex/ilocplex.h>

using namespace std;

int main(int argc, char **argv)
{
	int i, maxInf = 0;
	float obj = 0;
	Graph g;
	list<int> adj;
	float limite;
	ofstream r("SOLUTION.txt");
	while (0 == 0){

		cout << "(a) Tipo de grafo: ";
		cin >> maxInf;

		cout << "(b) Limite para pesos ignorados (de k a 1): ";
		cin >> limite;
		if (limite == -1){
			limite = 0;
		}
		g.loadFromFile(maxInf, limite);

		/*int or = 0, dt;

		while (or != -1){
			cout << "No de origem: ";
			cin >> or;
			cout << "No de destino: ";
			cin >> dt;
			cout << "Peso entre " << or << " e "<< dt << ": " << g.getWeight(or, dt) << " - Invertido: " << g.getInversedWeight(or, dt) << endl;
			}*/

		/*int k;
		for (k = 0; k < g.getNodes(); k++){
		g.getAdjacency(&adj, k + 1);
		cout << "No " << k+ 1 << " - Indices: ";
		while (!adj.empty()){
		cout << g.getSizesIndex(k + 1, adj.front()) << ", ";
		adj.pop_front();
		}
		cout << endl;
		}*/
		while (0 == 0){
			try {

				IloEnv env;
				IloModel model(env);
				IloBoolVarArray Y(env);
				IloNumVarArray W(env);
				IloNumVarArray X(env);

				cout << endl << "Informe o maximo de pessoas que receberao a informacao: ";
				cin >> maxInf;

				for (i = 0; i < g.getNodes(); i++){
					Y.add(IloBoolVar(env));
				}
				cout << "Variavel Y adicionada. Tamanho: " << Y.getSize() << endl;
				for (i = 0; i < g.getNodes(); i++){
					W.add(IloNumVar(env));
				}
				cout << "Variavel W adicionada. Tamanho: " << W.getSize() << endl;
				for (i = 0; i < g.getArcs(); i++){
					X.add(IloNumVar(env));
					if ((float)i / g.getArcs() > obj){
						cout << obj << endl;
						obj += 0.1;
					}
				}
				cout << "Variavel X adicionada. Tamanho: " << X.getSize() << endl;

				/*ofstream teste("indicesX.txt");
				int pl;
				for (i = 0; i < g.getNodes(); i++){
				g.getAdjacency(&adj, i + 1);
				while (!adj.empty()){
				pl = g.getSizesIndex(i + 1, adj.front());
				adj.pop_front();
				}
				}
				teste << "pl: " << pl << endl;
				teste << "\nTamanho do X: " << X.getSize() << endl;*/

				IloExpr objetivo(env);
				for (i = 0; i < W.getSize(); i++){
					objetivo += W[i];
				}
				//cout << "Objetivo: " << objetivo << endl;
				model.add(IloMaximize(env, objetivo));

				cout << "Funcao objetivo adicionada" << endl;

				IloExpr *somatorio = NULL;
				for (i = 0; i < W.getSize(); i++){
					model.add(W[i] <= 1);
				}
				cout << "Restricao 4 adicionada" << endl;
				somatorio = new IloExpr(env);
				for (i = 0; i < Y.getSize(); i++){
					*somatorio += Y[i];
				}
				model.add(*somatorio <= maxInf);
				delete somatorio;
				somatorio = NULL;
				cout << "Restricao 1 adicionada" << endl;
				

				int n;
				obj = 0;
				for (i = 0; i < g.getNodes(); i++){
					somatorio = new IloExpr(env);
					g.getAdjacency(&adj, i + 1);
					//cout << "n - " << i+1;
					while (!adj.empty()){
						n = adj.front();
						*somatorio += X[g.getSizesIndex(i + 1, n)];
						adj.pop_front();
					}
					//cout << " c\n";
					model.add(W[i] >= *somatorio);
					delete somatorio;
					somatorio = NULL;
					if ((float)i / g.getNodes() > obj){
						cout << obj << endl;
						obj += 0.01;
					}
				}
				cout << "Restricao 3 adicionada" << endl;

				obj = 0;
				for (i = 0; i < g.getNodes(); i++){
					somatorio = new IloExpr(env);
					*somatorio += Y[i];
					g.getAdjacency(&adj, i + 1);
					while (!adj.empty()){
						n = adj.front();
						*somatorio += X[g.getSizesIndex(n, i + 1)] * g.getInversedWeight(i + 1, n);
						adj.pop_front();
					}
					model.add(*somatorio >= W[i]);
					delete somatorio;
					somatorio = NULL;
					if ((float)i / g.getNodes() > obj){
						cout << obj << endl;
						obj += 0.01;
					}
				}
				cout << "Restricao 2 adicionada" << endl;

				IloCplex cplex(model);
				if (!cplex.solve()) {
					env.error() << "Failed to optimize LP." << endl;
					throw(-1);
				}

				IloNumArray valuesY(env), valuesW(env), valuesX(env);

				env.out() << "Solution status = " << cplex.getStatus() << endl;
				r << "Solution status = " << cplex.getStatus() << endl;
				env.out() << "Solution value = " << cplex.getObjValue() << endl;
				r << "Solution value = " << cplex.getObjValue() << endl;
				//cplex.getValues(valuesW, W);
				//cplex.getValues(valuesY, Y);
				//cplex.getValues(valuesX, X);
				//env.out() << "Values of W = " << valuesW << endl;
				//env.out() << "Values of Y = " << valuesY << endl;
				//env.out() << "Values of X = " << valuesX << endl;
				//env.out() << "People who received original information: " << endl;
				r << "People who received original information: " << endl;

				for (i = 0; i < Y.getSize(); i++){
					if (cplex.getValue(Y[i]) != 0){
						cout << i + 1 << ", ";
						r << i + 1 << endl;
					}
				}
				cout << endl << endl;

				cin >> i;
				env.end();
			}
			catch (IloException& e) {
				cerr << "Concert exception caught: " << e << endl;
			}
			catch (...) {
				cerr << "Unknown exception caught" << endl;
			}
		}
		cin >> i;
	}
	return 0;
}

