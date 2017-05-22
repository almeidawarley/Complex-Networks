// GETComp.cpp : Defines the entry point for the console application.
//
#include "graph.h"
#include "dictionary.h"
#include "stdafx.h"
#include <ilcplex/ilocplex.h>

using namespace std;

int main(int argc, char **argv)
{
	Graph graph;
	//Dictionary allowedNodes;
	//Index allowedArcs;
	int i, t, n;
	stringstream auxText;
	int maxInf;
	double carregamento = 0;
	list<int> adj;
	IloEnv env;

	cout << "Digite o grafo a ser utilizado: "; cin >> maxInf;

	graph.loadFromFile(maxInf, 0);
	Dictionary allowedNodes (graph.getNumberOfNodes());
	Index allowedArcs(graph.getNumberOfNodes());
	try {
			
		IloModel model(env);
		IloBoolVarArray Y(env);
		IloNumVarArray W(env);
		IloNumVarArray X(env);

		IloExprArray R2(env);
		IloExpr obj(env);
		IloExpr inf(env);
		
		cout << "Adicionando variaveis e primeiras restricoes:     ";
		for (i = 0; i < graph.getNumberOfNodes(); i++){
			if ((float)i / graph.getNumberOfNodes() > carregamento){
				carregamento += 0.01;
				printf("\b\b\b\b%3.f%%", carregamento*100);
			}
			if (graph.isConnected(i + 1)){
				t = allowedNodes.add(i + 1);

				//t = allowedNodes.getIndexByNode(i + 1);
				/* 
					Adiciona variavel W para cada no valido
				*/
				auxText << "w_" << i + 1;
				W.add(IloNumVar(env,0,1,auxText.str().c_str()));
				auxText.str("");
				/*
					Adiciona variavel Y para cada no valido
				*/
				auxText << "y_" << i + 1;
				Y.add(IloBoolVar(env, auxText.str().c_str()));
				auxText.str("");
				/*
					Cria restricao 2 e adiciona W, Y para no valido

				*/
				R2.add(IloExpr(env));
				R2[t] += Y[t]; 
				R2[t] -= W[t];
				
				/*
					Somatorio para a restricao 1 e funcao objetivo
				*/
				inf += Y[t]; 
				obj += W[t];
				
				graph.getAdjacency(&adj, i+1);
				while (!adj.empty()){
					t = adj.front();
					adj.pop_front();
					if (graph.isConnected(t)){
						auxText << "x_" << i + 1 << "_" << t;
						X.add(IloNumVar(env, 0, IloInfinity, auxText.str().c_str()));
						allowedArcs.add(i + 1, t);
						auxText.str("");
					}
				}
				/*
					No trecho acima, confiro se o no de destino do arco nao tem problema.
					Caso nao tenha, adiciono o arco a lista de arcos permitidos e crio uma variavel pro arco.
				*/
			}
		}
		cout << endl;
		/*
			Adiciono a funcao objetivo e restricao 1 para o somatorio feito acima.
		*/
		model.add(IloMaximize(env, obj));

		// allowedNodes.print(); cout << endl;
		//allowedArcs.print(); cout << endl;
		cout << "Adicionando restricoes nos arcos:     ";
		carregamento = 0;
		for (i = 0; i < allowedNodes.getSize(); i++){
			if ((float)i / allowedNodes.getSize() > carregamento){
				carregamento += 0.01;
				printf("\b\b\b\b%3.f%%", carregamento * 100);
			}

			int indexAux;
			n = allowedNodes.getNodeByIndex(i);
			graph.getAdjacency(&adj, n);
			while (!adj.empty()){
				t = adj.front();
				adj.pop_front();
				indexAux = allowedArcs.getIndex(t, n);
				if (indexAux != -1)
					R2[i] += graph.getWeight(n, t)*X[indexAux];
				
				indexAux = allowedArcs.getIndex(n,t);
				if (indexAux != -1){
					model.add(W[i] >= X[indexAux]);
					/*
						Restricao 3 para todo no valido
					*/
				}
				
			}
			model.add(R2[i] >= 0);
			
		}
		cout << endl;

		/*int origem = 1, destino = -1;
		while (origem != destino){
			cout << "Explore o index dos arcos. Digite o arco que deseja explorar: ";
			cin >> origem;
			allowedArcs.print(origem);
		}*/

		allowedArcs.free();
		graph.setIndexes(&allowedNodes);
		allowedNodes.free();

		
		while (0 == 0){
			IloModel submodel(env);
			submodel.add(model);
			cout << "Digite o maximo de informacao difundida: "; cin >> maxInf;
			submodel.add(inf <= maxInf);

			IloCplex cplex(submodel);
			cplex.exportModel("modelo.lp");
			if (!cplex.solve()) {
				env.error() << "Failed to optimize LP." << endl;
				throw(-1);
			}

			IloNumArray valuesY(env), valuesW(env), valuesX(env);

			
			/*cplex.getValues(valuesW, W);
			cplex.getValues(valuesY, Y);
			cplex.getValues(valuesX, X);
			env.out() << "Values of W = " << valuesW << endl;
			env.out() << "Values of Y = " << valuesY << endl;
			env.out() << "Values of X = " << valuesX << endl;*/
			env.out() << "********************************************" << endl;
			env.out() << "maxInf: " << maxInf << endl;
			env.out() << "Solution status = " << cplex.getStatus() << endl;
			env.out() << "Solution value = " << cplex.getObjValue() << endl;
			env.out() << "People who received original information: " << endl;
			/*for (i = 0; i < Y.getSize(); i++){
				if (cplex.getValue(Y[i]) > 0.001){
					cout << allowedNodes.getNodeByIndex(i) << " - ";
					cout << graph.getSumOfInversedWeights(allowedNodes.getNodeByIndex(i)) << ", ";
				}
			}*/
			for (i = 0; i < graph.getNumberOfNodes(); i++){
				n = graph.getIndexByNode(i + 1);
				if (n!=-1&&cplex.getValue(Y[n]) > 0.001){
					env.out() << "[" << i + 1 << "]: " << endl;
					env.out() << " | Soma dos pesos dos arcos que chegam : " << graph.getSumOfInversedWeights(i + 1) << endl;
					//cout << " | Soma dos pesos dos arcos que saem : " << graph.getSumOfWeights(i + 1) << endl;
					//cout << " | Numero de arcos que saem : " << graph.getNumberOfArcs(i + 1) << endl;
					//cout << " | Numero de arcos que chegam : " << graph.getNumberOfArcsReceived(i + 1) << endl;
					/*cout << " | Adjacencia direta: "; 
					graph.getAdjacency(&adj, i + 1);
					while (!adj.empty()){
						if (graph.isConnected(adj.front()))
							cout << adj.front() << ", ";
						adj.pop_front();
					}
					cout << endl;
					cout << " | Adjacencia inversa : ";
					graph.getInversedAdjacency(&adj, i + 1);
					while (!adj.empty()){
						if (graph.isConnected(adj.front()))
							cout << adj.front() << ", "; 
						adj.pop_front();
					}
					cout << endl;*/
				}
			}
			env.out() << endl << "********************************************" << endl;
			env.out() << endl << endl;
			//allowedNodes.print(); cout << endl;
			//allowedArcs.print(); cout << endl;
		}
			
	}
	catch (IloException& e) {
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
	env.end();
	cout << "Acabou! :)" << endl;
	cin >> maxInf;

	return 0;
}

