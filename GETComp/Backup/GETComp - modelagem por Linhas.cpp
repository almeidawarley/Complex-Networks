// GETComp.cpp : Defines the entry point for the console application.
//
#include "graph.h"
#include "stdafx.h"
#include <ilcplex/ilocplex.h>

using namespace std;

float roundNumber(float f, int c){
	int inteiro;
	inteiro = (int)pow(10,c) * f;
	f = (float)inteiro / pow(10, c);
	return f;
}

int main(int argc, char **argv)
{
	int controle = 0;
	int controleMaxInf = 0, passoMaxInf = 1, maximoMaxInf = 1000;
	float passoLimite = 0.1,  controleLimite = 0.7;
	cout << "**********************************************************" << endl;
	cout << "Para cada loop, informe:\n(a) 0 ou 1 para grafo utilizado, \n(b) o limite inferior de pesos\n(c) numero de pessoas com info original" << endl;
	cout << "**********************************************************" << endl;

	while (controleLimite>=0){
		int i, maxInf = 0;
		float obj = 0;
		Graph g;
		list<int> adj;
		float limite;
		IloEnv env;
		cout << "(a) Tipo de grafo: ";
		cin >> maxInf;
		stringstream nome;
		
		//maxInf = 1;
		cout << "(b) Limite para pesos ignorados (de k a 1): ";
		cin >> limite;
		if (limite == -1){
			limite = 0;
		}
		//limite = controleLimite;
		//controleLimite -= passoLimite;
		g.loadFromFile(maxInf, limite);

		/*int a = 0, b;
		while (a != -1){
			cout << "No de origem: ";
			cin >> a;
			cout << "No de destino: ";
			cin >> b;
			cout << "Index entre " << a << " e " << b << ": Index de ida" << g.getSizesIndex(a, b) << " - Index de volta: " << g.getSizesIndex(b,a) << endl;
		}
		*/

		/*int or = 0, dt;

		while (or != -1){
			cout << "No de origem: ";
			cin >> or;
			cout << "No de destino: ";
			cin >> dt;
			cout << "Peso entre " << or << " e "<< dt << ": " << g.getWeight(or, dt) << " - Invertido: " << g.getInversedWeight(or, dt) << endl;
			}
		*/
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
		cout << "Numero de arcos: " << g.getArcs() << endl;
		cout << "Numero de nos: " << g.getNodes() << endl;
		/*for (i = 0; i < g.getNodes(); i++){
			g.getAdjacency(&adj, i + 1);
			while (!adj.empty()){
				//cout << "Origem: " << i + 1 << " - Destino: " << adj.front() << " - Index: " << g.getSizesIndex(i + 1, adj.front()) << " - Peso: " << g.getWeight(i + 1, adj.front()) << endl;
				cout << g.getSizesIndex(i + 1, adj.front())<< ", ";
				adj.pop_front();
			}
		}*/
		/*cout << "Adjacencia normal" << endl;
		for (i = 0; i < g.getNodes(); i++){
			g.getAdjacency(&adj, i + 1);
		}
		cout << "Adjacencia invertida" << endl;
		for (i = 0; i < g.getNodes(); i++){
			g.getInversedAdjacency(&adj, i + 1);
		}*/
		
		cout << "Iniciando com limite " << limite << "..." << endl;
		try {
			
			IloModel model(env);
			IloBoolVarArray Y(env);
			IloNumVarArray W(env);
			IloNumVarArray X(env);

			for (i = 0; i < g.getNodes(); i++){
				Y.add(IloBoolVar(env));
				nome.str("");
				nome << "y_" << i+1;
				Y[i].setName(nome.str().c_str());
			}
			cout << "Variavel Y adicionada. Tamanho: " << Y.getSize() << endl;
			for (i = 0; i < g.getNodes(); i++){
				W.add(IloNumVar(env));
				nome.str("");
				nome << "w_" << i+1;
				W[i].setName(nome.str().c_str());
			}
			cout << "Variavel W adicionada. Tamanho: " << W.getSize() << endl;

			int k = 0;
			for (i = 0; i < g.getNodes(); i++){
				g.getAdjacency(&adj, i + 1);
				int j;
				while (!adj.empty()){
					j = adj.front();
					adj.pop_front();

					X.add(IloNumVar(env));
					if ((float) i / g.getNodes() > obj){
						cout << obj << endl;
						obj += 0.01;
					}
					nome.str("");
					nome << "x_" << i+1 << "_" << j;
					X[k++].setName(nome.str().c_str());
				}
			}
			/*for (i = 0; i < g.getArcs(); i++){
				X.add(IloNumVar(env));
				nome.str("");
				nome << "x_" << i;
				X[i].setName(nome.str().c_str());
			}*/
			cout << "Variavel X adicionada. Tamanho: " << X.getSize() << endl;
			cout << "Numero de arcos: " << g.getArcs() << endl;
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
				

			int n;
			obj = 0;
			cout << "Restricao 3:    ";
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
				cout << i + 1 << endl;
				if ((float)i / g.getNodes() > obj){
					//cout << obj << endl;
					printf("\b\b\b\b%03.0f%%", obj*100);
					obj += 0.01;
				}
			}
			cout << "\b\b\b\b\b adicionada" << endl;

			obj = 0;
			cout << "Restricao 2:    ";
			for (i = 0; i < g.getNodes(); i++){
				//cout << "No " << i + 1 << ": " << endl;
				somatorio = new IloExpr(env);
				*somatorio += Y[i];
				//cout << "variavel Y; " << endl;
				g.getInversedAdjacency(&adj, i + 1);
				while (!adj.empty()){
					n = adj.front();
					//cout << "g.getSizesIndex(" << n << ", " << i+1 << ") = " << g.getSizesIndex(n, i + 1) << endl;
					//cout << "g.getInversedWeight(" << i+1 << ", " << n << ") = " << g.getInversedWeight(i + 1, n) << endl;
					
					//if (g.getSizesIndex(n, i+1) != -1)
					*somatorio += X[g.getSizesIndex(n, i+1)] * roundNumber(g.getInversedWeight(i + 1, n), 2);
					//cout << "variavel X_" << i + 1 << "." << n << ", " << endl;
					adj.pop_front();
				}
				model.add(*somatorio >= W[i]);
				//cout << "variavel W" << endl;
				delete somatorio;
				somatorio = NULL;
				if ((float)i / g.getNodes() > obj){
					//cout << obj << endl; 
					printf("\b\b\b\b%03.0f%%", obj * 100);
					obj += 0.01;
				}
			}
			cout << "\b\b\b\b\b adicionada" << endl;
			controleMaxInf = 0;
			maxInf = 0;
			while (maximoMaxInf>maxInf){
				
				stringstream nomeSS;
				
				IloModel submodel(env);
				submodel.add(model);

				cout << endl << "Informe o maximo de pessoas que receberao a informacao: ";
				cin >> maxInf;
				//maxInf = controleMaxInf;
				//controleMaxInf += passoMaxInf;
				nomeSS << "lim[" << limite << "]_maxInf[" << maxInf << "].txt";
				ofstream r(nomeSS.str());				
				r << "maxInf " << maxInf << endl;
				r << "limite " << limite << endl;

				somatorio = new IloExpr(env);
				for (i = 0; i < Y.getSize(); i++){
					*somatorio += Y[i];
				}
				submodel.add(*somatorio <= maxInf);
				delete somatorio;
				somatorio = NULL;
				cout << "Restricao 1 adicionada" << endl;


				IloCplex cplex(submodel);
				cplex.exportModel("modelo.lp");
				if (!cplex.solve()) {
					env.error() << "Failed to optimize LP." << endl;
					throw(-1);
				}

				IloNumArray valuesY(env), valuesW(env), valuesX(env);

				env.out() << "Solution status = " << cplex.getStatus() << endl;
				r << "Solution status = " << cplex.getStatus() << endl;
				env.out() << "Solution value = " << cplex.getObjValue() << endl;
				r << "Solution value = " << cplex.getObjValue() << endl;
				cplex.getValues(valuesW, W);
				cplex.getValues(valuesY, Y);
				cplex.getValues(valuesX, X);
				env.out() << "Values of W = " << valuesW << endl;
				env.out() << "Values of Y = " << valuesY << endl;
				env.out() << "Values of X = " << valuesX << endl;
				env.out() << "People who received original information: " << endl;
				r << "People who received original information: " << endl;
				cout << "********************************************" << endl;
				cout << "maxInf: " << maxInf << " limite: " << limite << endl;
				cout << "People who received original information: ";
				for (i = 0; i < Y.getSize(); i++){
					if (cplex.getValue(Y[i]) > 0.001){
						cout << i + 1 << ", ";
						r << i + 1 << endl;
					}
				}
				cout <<endl<< "********************************************" << endl;
				cout << endl << endl;
				//cout << "Deseja rodar novamente? Digite 0 para sim e -1 para nao: ";
				//cin >> controle;
			}
			
		}
		catch (IloException& e) {
			cerr << "Concert exception caught: " << e << endl;
		}
		catch (...) {
			cerr << "Unknown exception caught" << endl;
		}
		env.end();
	}
	cout << "Acabou! :)" << endl;
	cin >> maximoMaxInf;
	return 0;
}

