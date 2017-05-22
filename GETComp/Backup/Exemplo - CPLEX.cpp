// GETComp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ilcplex/ilocplex.h>

using namespace std;
int main(int argc, char **argv)
{
	int i;
	IloEnv env;
	try {
		IloModel model(env);
		IloNumVarArray vars(env);

		
		vars.add(IloNumVar(env, 0.0, 40.0));
		vars.add(IloNumVar(env));
		vars.add(IloNumVar(env));

		IloExpr objetivo(env);
		double custo[3] = { 1, 2, 3 };
		for (i = 0; i < vars.getSize(); i++){
			objetivo += custo[i] * vars[i];
		}
		model.add(IloMaximize(env, objetivo));

		IloExpr restricao1(env), restricao2(env);
		double custor1[3] = { -1, 1, 1 };
		for (i = 0; i < vars.getSize(); i++){
			restricao1 += custor1[i] * vars[i];
		}

		double custor2[3] = {1, -3, 1 };
		for (i = 0; i < vars.getSize(); i++){
			restricao2 += custor2[i] * vars[i];
		}

		model.add(restricao1 <= 20);
		model.add(restricao2 <= 30);

		IloCplex cplex(model);
		if (!cplex.solve()) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}

		IloNumArray vals(env);
		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value = " << cplex.getObjValue() << endl;
		cplex.getValues(vals, vars);
		env.out() << "Values = " << vals << endl;
	}
	catch (IloException& e) {
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
	
	env.end();
	cin >> i;

	return 0;
}

