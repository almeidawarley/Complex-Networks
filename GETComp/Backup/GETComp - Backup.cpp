// GETComp.cpp : Defines the entry point for the console application.
//
#include "graph.h"
#include "stdafx.h"
#include <ilcplex/ilocplex.h>

using namespace std;

int main(int argc, char **argv)
{
	int i, j, maxInf = 0;
	IloEnv env;
	Graph g;
	list<int> adj;
	g.loadFromFile(0);
	g.shortestPath(1, 5);


	while (0 == 0){
	try {
		IloModel model(env);
		IloBoolVarArray Y(env);
		IloNumVarArray W(env);

		
			cout << endl << "Informe o maximo de pessoas que receberao a informacao: ";
			cin >> maxInf;
			/*int c = 0;
			for (c = 0; c < g.getNodes(); c++){
			cout << endl << "Adjacencia de " << c+1 << ": " << endl;
			g.getAdjacency(&adj, c+1);
			while (!adj.empty()){
			cout << adj.front() << ", ";
			adj.pop_front();
			}
			cout << endl;
			}*/

			for (i = 0; i < g.getNodes(); i++){
				Y.add(IloBoolVar(env));
			}
			cout << "Variavel Y adicionada. Tamanho: " << Y.getSize() << endl;
			for (i = 0; i < g.getNodes(); i++){
				W.add(IloNumVar(env));
			}
			cout << "Variavel W adicionada. Tamanho: " << W.getSize() << endl;

			IloExpr objetivo(env);
			for (i = 0; i < W.getSize(); i++){
				objetivo += W[i];
			}
			//cout << "Objetivo: " << objetivo << endl;
			model.add(IloMaximize(env, objetivo));

			cout << "Funcao objetivo adicionada" << endl;

			IloExpr *restricao = NULL, *somatorio = NULL;
			for (i = 0; i < W.getSize(); i++){
				model.add(W[i] <= 1);
				model.add(W[i] >= 0);
			}
			cout << "Restricao 4 adicionada" << endl;
			restricao = new IloExpr(env);
			for (i = 0; i < Y.getSize(); i++){
				*restricao += Y[i];
			}
			model.add(*restricao <= maxInf);
			delete restricao;
			restricao = NULL;
			cout << "Restricao 1 adicionada" << endl;

			int n;
			for (i = 0; i < Y.getSize(); i++){
				restricao = new IloExpr(env);
				*restricao += Y[i];
				g.getAdjacency(&adj, i + 1);
				// Código melhor, pois só procura na sua adjacencia
				while (!adj.empty()){
					n = adj.front();
					*restricao += W[n - 1] * g.getInversedWeight(i + 1, n);
					adj.pop_front();
				}
				/* // Código ruim, pois toda vez passa por todos os nós
				
				for (j = 0; j < W.getSize(); j++){
					*restricao += W[j] * g.getInversedWeight(i+1, j+1);
				}*/
				//cout << "Adicionando restricao do " << i + 1 << endl;

				model.add(*restricao >= W[i]);
				//cout << *restricao << endl;
				delete restricao;
				restricao = NULL;
			}
			cout << "Restricao 2 adicionada" << endl;

			for (i = 0; i < W.getSize(); i++){
				restricao = new IloExpr(env);
				g.getAdjacency(&adj, i + 1);
				while (!adj.empty()){
					n = adj.front();
					*restricao += g.getWeight(i + 1, n);
					adj.pop_front();
				}
				/*for (j = 0; j < W.getSize(); j++){
					*restricao += g.getWeight(i + 1, j+1);
				}*/
				model.add(W[i] >= *restricao);
				delete restricao;
				restricao = NULL;
			}
			cout << "Restricao 3 adicionada" << endl;

			IloCplex cplex(model);
			if (!cplex.solve()) {
				env.error() << "Failed to optimize LP." << endl;
				throw(-1);
			}

			IloNumArray valuesY(env), valuesW(env);
			env.out() << "Solution status = " << cplex.getStatus() << endl;
			env.out() << "Solution value = " << cplex.getObjValue() << endl;
			cplex.getValues(valuesW, W);
			cplex.getValues(valuesY, Y);
			env.out() << "Values of W = " << valuesW << endl;
			env.out() << "Values of Y = " << valuesY << endl;
		}
		catch (IloException& e) {
			cerr << "Concert exception caught: " << e << endl;
		}
		catch (exception e) {
			e.what();
			cerr << "Unknown exception caught" << endl;
		}

		env.end();
		cin >> i;
	}
	return 0;
}

