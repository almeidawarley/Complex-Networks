// GETComp.cpp : Defines the entry point for the console application.
//
#include "graph.h"
#include "stdafx.h"
#define PARAM 20
#define RANGE 10
#include <ilcplex/ilocplex.h>

using namespace std;

void createColumn(Dictionary *allowedNodes, Tree *tree, IloRangeArray *R);
bool chooseNode(Graph *g, Dictionary *n, IloRangeArray *r, IloBoolVarArray z, int parameter, int range);



bool chooseNode(Graph *g, Dictionary *n, IloRangeArray *r, IloBoolVarArray z, int parameter, int range){
	/*for (int c = 0; c < g->getNumberOfNodes(); c++){
		int f = g->getInitialVertexes(parameter + c)->front();
		if (g->depthSearch(f)->size() > range){
			Tree *t = g->breadthSearch(f);
			
			}
			return true;
		}
	}
	return false;*/
	return true;
}


int main(int argc, char **argv){

	IloEnv env;
	Graph graph;
	Tree *tree;
	list<int> adj, *allowedZ;
	graph.loadFromFile(0, 0);
	Dictionary allowedNodes(graph.getNumberOfNodes());
	stringstream auxText;
	double carregamento = 0;
	int t;
	int maxInf;
	
	try {

		IloModel model(env);
		IloNumVarArray W(env);
		IloBoolVarArray Z(env);

		IloExpr objetivo(env);
		IloExpr somatorio(env);
		IloRangeArray R(env);
		IloExpr *expressao;

		float sum = 0;
		cout << "Carregando variaveis:      ";
		for (int u = 0; u < graph.getNumberOfNodes(); u++){
			if (graph.isConnected(u + 1)){
				auxText << "w_" << u + 1;
				W.add(IloNumVar(env, 0, 1, auxText.str().c_str()));
				auxText.str("");
				auxText << "z_" << u + 1;
				Z.add(IloBoolVar(env, 0, 1, auxText.str().c_str()));
				auxText.str("");
				t = allowedNodes.add(u + 1);
				objetivo += W[t];
				somatorio += Z[t];
				auxText << "r_" << u + 1;
				R.add(IloRange(env, -IloInfinity, W[t] - Z[t], 0, auxText.str().c_str()));
				auxText.str("");
				if ((float)u / graph.getNumberOfNodes() - carregamento > 0.01){
					carregamento = (float)u / graph.getNumberOfNodes();
					printf("\b\b\b\b%3.f%%", carregamento * 100);
				}
			}
		}
		cout << endl;

		model.add(IloMaximize(env, objetivo));

		cout << "Carregando restricoes:      ";
		carregamento = 0;
		allowedZ = graph.getInitialVertexes(PARAM);
		while (!allowedZ->empty()){
			int n = allowedZ->front();
			allowedZ->pop_front();
			tree = graph.breadthSearch(n);
			createColumn(&allowedNodes, tree, &R);


			int u = allowedNodes.getIndexByNode(n);
			for (int f = 0; f < allowedNodes.getSize(); f++){
				t = allowedNodes.getNodeByIndex(f);
				if (tree->getInfoByVertex(t) != 0){
					expressao = new IloExpr(env);
					*expressao = R[f].getExpr();
					*expressao -= Z[u] * tree->getInfoByVertex(t);
					R[f].setExpr(*expressao);
					delete expressao;
				}
			}
			delete tree;
		}
		cout << endl << "Maximo de informacao difundida: ";
		cin >> maxInf;
		model.add(somatorio <= maxInf);

		IloNumArray valuesZ(env), valuesW(env);

		do{
			IloModel submodel(env);
			submodel.add(model);
			submodel.add(R);

			IloCplex cplex(submodel);
			cplex.exportModel("modelo.lp");
			if (!cplex.solve()) {
				env.error() << "Failed to optimize LP." << endl;
				throw(-1);
			}

			env.out() << "********************************************" << endl;
			env.out() << "maxInf: " << maxInf << endl;
			env.out() << "Solution status = " << cplex.getStatus() << endl;
			env.out() << "Solution value = " << cplex.getObjValue() << endl;
			env.out() << "People who received original information: " << endl;
			cplex.getValues(W, valuesW);
			cplex.getValues(Z, valuesZ);
			//env.out() << valuesW << endl;

			for (int i = 0; i < allowedNodes.getSize(); i++){
				int n = allowedNodes.getNodeByIndex(i);
				if (fabs(cplex.getValue(Z[i])) > 0.001){
					env.out() << "[" << n << "]: " << endl;
					env.out() << " | Soma dos pesos dos arcos que chegam : " << graph.getSumOfInversedWeights(i + 1) << endl;
				}
			}
			env.out() << endl << "********************************************" << endl;
			env.out() << endl << endl;
			cin >> maxInf;
		} while (chooseNode(&graph, &allowedNodes, &R, Z, PARAM, RANGE));
	}
	catch (IloException& e) {
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
	env.end();
	cout << "Acabou! :)" << endl;
	return 0;
}

