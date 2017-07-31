// GETComp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <ilcplex/ilocplex.h>

#define HI 99999
#define LI -99999
using namespace std;
Funct utilities;
LARGE_INTEGER t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, freq;

int time(LARGE_INTEGER *t2, LARGE_INTEGER *t1, LARGE_INTEGER *f){
	return (int)(((t2->QuadPart - t1->QuadPart) * 1000) / f->QuadPart);
}

void createColumn(Graph *graph, bool *generatedColumns, int node, Dictionary *allowedNodes, IloRangeArray R, IloNumVarArray Z, IloNumVarArray W, IloObjective objective, IloRange infFunction, double cut, int model, bool option = false){
	Tree tree;
	graph->breadthSearch(&tree, node, cut);
	register int index = allowedNodes->getIndexByNode(node);
	if (generatedColumns[index]){
		cout << "Coluna: " << node << endl;
		utilities.wait("Erro ao gerar colunas");
	}
	generatedColumns[index] = true;
	double control = 0;
	int current;
	int dIndex; //register
	for (int f = 0; f < tree.getSize(); f++){
		current = tree.nodes[f];
		dIndex = allowedNodes->getIndexByNode(current);
		if (option)
			utilities.load(f, tree.getSize(), &control);
		if ((model == 2 || model == 3) && tree.info[f] > 0){
			//R[dIndex].setLinearCoef(Z[index], -1);
			R[dIndex].setExpr(R[dIndex].getExpr() - Z[index]);
		}
		else if ((model == 1 || model == 4) && tree.getInfoByVertex(current) != 0){
			//R[dIndex].setLinearCoef(Z[index], (double) -1 * tree.info[f]);
			R[dIndex].setExpr(R[dIndex].getExpr() - tree.info[f]*Z[index]);
		}
	}
	R[index].setLinearCoef(Z[index], -1);
}

void populate(IloEnv env, Graph *graph, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R, IloObjective objective, int model){
	double control = 0;
	stringstream auxText;
	Funct utilities;
	cout << "> Creating variables:      ";
	for (int u = 0; u < graph->getNumberOfNodes(); u++){
		if (graph->isConnected(u + 1)){
			auxText << "w(" << u + 1<<")";
			switch (model){
			case 1: W.add(IloNumVar(env, 0, 1, auxText.str().c_str())); break;
			case 2: W.add(IloBoolVar(env, 0, 1, auxText.str().c_str())); break;
			case 3: W.add(IloBoolVar(env, 0, 1, auxText.str().c_str())); break;
			case 4: W.add(IloNumVar(env, 0, 1, auxText.str().c_str())); break;
			default: cout << "Wrong model parameter" << endl;
			}
			auxText.str("");
			auxText << "z(" << u + 1 << ")";
			Z.add(IloBoolVar(env, 0, 1, auxText.str().c_str()));
			auxText.str("");
			int index = allowedNodes->add(u + 1);
			switch (model){
			//case 1: objective += W[index]; break;
			//case 2: objective += W[index]; break;
			//case 3: objective += Z[index]; break;
			//case 4: objective += W[index]; break;
			case 1: objective.setLinearCoef(W[index],1); break;
			case 2: objective.setLinearCoef(W[index], 1); break;
			case 3: objective.setLinearCoef(Z[index], 1); break;
			case 4: objective.setLinearCoef(W[index], 1); break;
			default: cout << "Wrong model parameter"; break;
			}
			auxText << "r_" << u + 1;
			R.add(IloRange(env, -IloInfinity, W[index], 0, auxText.str().c_str()));
			auxText.str("");
			utilities.load(u, graph->getNumberOfNodes(), &control);
		}
	}
	cout << endl;
}

void createColumns(IloEnv env, Graph *graph, bool *generatedColumns, Dictionary *allowedNodes, IloRangeArray R, IloNumVarArray Z, IloNumVarArray W, IloObjective objective, IloRange funcaoInf, double cut, int amount, int orderingBy, int model){
	list<int> allowedZ;
	cout << "> Choosing first nodes to generate columns:      ";
	graph->getInitialVertexes(&allowedZ, orderingBy, amount);
	cout << endl;
	cout << "> Creating columns:      ";
	double control = 0;
	int initialSize = (int) allowedZ.size();
	while (!allowedZ.empty()){
		createColumn(graph, generatedColumns, allowedZ.front(), allowedNodes, R, Z, W, objective, funcaoInf, cut, model, false);
		allowedZ.pop_front();
		utilities.load(initialSize - (int) allowedZ.size(), initialSize, &control);
	}
	cout << endl << endl;
}

void parameters(IloCplex cplex, int initialC, int initialR, int timelimit){
	cplex.setParam(IloCplex::TiLim, timelimit* 60);
	cplex.setParam(IloCplex::MemoryEmphasis, true);
	cplex.setParam(IloCplex::WorkMem, 100);
	cplex.setParam(IloCplex::NodeFileInd, 1);
	cplex.setParam(IloCplex::ColReadLim, initialC);
	cplex.setParam(IloCplex::RowReadLim, initialR);
	cplex.setParam(IloCplex::NumericalEmphasis, 1);
	//cout <<  "**************************************************************************" << endl;
	//cout << " | Time limit: " << cplex.getParam(IloCplex::TiLim)  << " seconds" << endl;
	//cout << " | Memory emphasis: " << cplex.getParam(IloCplex::MemoryEmphasis) << endl;
	//cout << " | Workable memory: " << cplex.getParam(IloCplex::WorkMem) << " MBs" << endl;
	//cout << " | MIP Emphashis: " << cplex.getParam(IloCplex::MIPEmphasis) << endl;
	//cout << " | NodeFileInd: " << cplex.getParam(IloCplex::NodeFileInd) << endl;
	//cout << " | ColReadLim: " << cplex.getParam(IloCplex::ColReadLim) << endl;
	//cout << " | RowReadLim: " << cplex.getParam(IloCplex::RowReadLim) << endl;
	//cout << "**************************************************************************" << endl;
}

// Z binário, W fracionário, maximiza quantidade de informação recebida (somatório de W) atendendo no máximo maxInf pessoas
void model001(IloModel model, float parameter, IloObjective objective, IloRange infFunction, IloNumVarArray W, IloNumVarArray Z){
	try{
		IloEnv env = model.getEnv();
		infFunction.setExpr(IloSum(Z));
		infFunction.setUb(parameter);
		objective.setExpr(IloSum(W));
	}
	catch (IloException e){
		cerr << e << endl;
	}
}

// Z binário, W binário, maximiza número de pessoas alcançadas (somatório de W) atendendo no máximo maxInf pessoas
void model002(IloModel model, float parameter, IloObjective objective, IloRange infFunction, IloNumVarArray W, IloNumVarArray Z) {
	try{	
		IloEnv env = model.getEnv();
		infFunction.setUb(parameter);
		infFunction.setExpr(IloSum(Z));
		objective.setExpr(IloSum(W));
	}
	catch (IloException e){
		cerr << e << endl;
	}
}

// Z binário, W binário, minimiza número de pessoas escolhidas inicialmente (somatório de Z) atendendo no mínimo uma porcentagem da rede
void model003(IloModel model, float parameter, IloObjective objective, IloRange infFunction, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z){
	try{
		IloEnv env = model.getEnv();
		infFunction.setLb(allowedNodes->getSize()*parameter);
		infFunction.setExpr(IloSum(W));
		objective.setExpr(IloSum(Z));
	}
	catch (IloException e){
		cerr << e << endl;
	}
}

void model004(IloModel model, float parameter, IloObjective objective, IloRange infFunction, IloNumVarArray W, IloNumVarArray Z, IloNumArray solucao){
	try{
		IloEnv env = model.getEnv();
		infFunction.setExpr(-IloSum(Z));
		objective.setExpr(IloSum(W));
		infFunction.setLb(-IloInfinity);
		for (int i = 0; i < solucao.getSize(); i++)
			if (solucao[i] > 0.9)
				model.add(Z[i] == 1);

	}
	catch (IloException e){
		cerr << e << endl;

	}
}

void localBranching(IloRange localB, IloNumArray solucao, IloNumVarArray Z,  Dictionary *allowedNodes, int radius){
	cout << "Adding local branching constraint" << endl;
	int counter = 0;
	for (int u = 0; u < allowedNodes->getSize(); u++){
		counter += solucao[u];
	}
	localB.setLinearCoefs(Z, solucao);
	localB.setUB(counter- 1);
}

void solve(int modelNumber, Graph *graph, Dictionary *allowedNodes, IloModel model, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R, IloRangeArray T, IloObjective objective, IloRange infFunction, IloRange localB, float parameter, float infCut, int initialC, int timelimit, string append, IloNumArray solucao, bool branching = false){

	IloEnv env = model.getEnv();
	ofstream dataOutput(append.c_str(), ios::app);
	stringstream auxText;
	IloCplex cplex(model);

	QueryPerformanceCounter(&t0);
	QueryPerformanceFrequency(&freq);

	//QueryPerformanceCounter(&t1);
	//populate(env, graph, allowedNodes, W, Z, R, objective, modelNumber);
	//QueryPerformanceCounter(&t2);

	bool *generatedColumns = new bool[allowedNodes->getSize()];
	utilities.setBool(generatedColumns, allowedNodes->getSize(), false);

	QueryPerformanceCounter(&t3);
	createColumns(env, graph, generatedColumns, allowedNodes, R, Z, W, objective, infFunction, infCut, initialC, 1, modelNumber);
	QueryPerformanceCounter(&t4);


	switch (modelNumber){
	case 1: IloConversion(env, W, ILOFLOAT, "m1"); objective.setSense(IloObjective::Maximize); break;
	case 2: IloConversion(env, W, ILOINT, "m2"); objective.setSense(IloObjective::Maximize); break;
	case 3: IloConversion(env, W, ILOINT, "m3"); objective.setSense(IloObjective::Minimize); break;
	case 4: IloConversion(env, W, ILOFLOAT, "m4"); objective.setSense(IloObjective::Maximize); break;
	}

	QueryPerformanceCounter(&t5);
	
	switch (modelNumber){
		case 1: model001(model, parameter, objective, infFunction, W, Z); break;
		case 2: model002(model, parameter, objective, infFunction, W, Z); break;
		case 3: model003(model, parameter, objective, infFunction, allowedNodes, W, Z); break;
		case 4: model004(model, parameter, objective, infFunction, W, Z, solucao); break;
		default: cout << "Wrong model parameter" << endl; exit(1);
	}
	QueryPerformanceCounter(&t6);
	IloNumVar folga(env);	

	bool repeat;
	int previousSolution = MAXINT;
	int iteration = 0;
	do{
		bool hasPreviousSolution = false;
		repeat = false;
		iteration++;

		QueryPerformanceCounter(&t7);
		if (branching)
			timelimit = 5; //if using local braching we're going to wait 5 minutes for better solutions
		parameters(cplex, initialC, allowedNodes->getSize(), timelimit);
		
		if (solucao[0] != -1){
			if (branching){
				localBranching(localB, solucao, Z, allowedNodes, 1);
				localB.setLinearCoef(folga, -1);
				objective.setLinearCoef(folga, 500);
				cout << "Local branching constraint added to the model" << endl;
			}
			try{
				cplex.addMIPStart(Z, solucao, IloCplex::MIPStartAuto, "previousSolution");
			}
			catch (IloException e){
				cout << "Previous solution could not be used by solver" << endl;
				hasPreviousSolution = false;
				cerr << e << endl;
			}
		}
		try{
			cplex.solve();
		}
		catch (IloException e){
			cerr << e << endl;
		}
		if (!cplex.solve()) {
			env.error() << "Failed to optimize LP." << endl;
			auxText << "error00" << modelNumber << ".lp";
			ofstream error(auxText.str().c_str());
			error << model << endl;
			auxText.str("");
			//throw(-1);
		}
		else{
			try{
				QueryPerformanceCounter(&t8);

				auxText << "model00" << modelNumber << ".lp";
				cplex.exportModel(auxText.str().c_str());
				auxText.str("");

				auxText << "error00" << modelNumber << ".lp";
				ofstream error(auxText.str().c_str());
				error << infFunction << endl;
				error << objective << endl;
				auxText.str("");

				if (previousSolution >= cplex.getObjValue()){
					repeat = true;
					previousSolution = cplex.getObjValue();
				}

				QueryPerformanceCounter(&t9);
				stringstream ids;
				ids << "{";
				IloNumArray valuesW(env);
				IloNumArray valuesZ(env);
				cplex.getValues(Z, valuesZ);
				cplex.getValues(W, valuesW);
				int sTreeSize = 0, minTreeSize = HI, maxTreeSize = LI;
				float sInversedWeight = 0, minInversedWeight = HI, maxInversedWeight = LI;
				solucao.clear();

				IloNum solutionValue = cplex.getObjValue();
				int solutionIndex = 0;
				int reached = 0;
				int inSolution = 0;
				int counter = 0;
				env.out() << endl << "**************************************************************************" << endl;
				env.out() << " | MODEL 00" << modelNumber << endl;
				switch (modelNumber){
				case 1: env.out() << " | maxInf: " << parameter; break;
				case 2: env.out() << " | maxInf: " << parameter; break;
				case 3: env.out() << " | percentage: " << parameter * 100 << "%"; break;
				}
				env.out() << "\tSolution status = " << cplex.getStatus() << "\tSolution value = " << solutionValue << endl;
				if (branching)
					cout << "Value of folga : " << cplex.getValue(folga) * 500 << endl;
				env.out() << " > People who received original information:" << endl;
				for (int i = 0; i < allowedNodes->getSize(); i++){
					int node = allowedNodes->getNodeByIndex(i);
					if (generatedColumns[i] && cplex.getValue(Z[i]) > 0.9){
						ids << node << " - ";
						solucao.add(1);
						inSolution++;
						Tree tree;
						graph->breadthSearch(&tree, node, infCut);
						env.out() << " | ID:  " << node << "\tSIW: " << graph->getSumOfInversedWeights(node) << "\tTree size:  " << tree.getSize() << "\tI: " << i << endl;
						sInversedWeight += graph->getSumOfInversedWeights(node);
						sTreeSize += tree.getSize();
						if (tree.getSize() < minTreeSize)
							minTreeSize = tree.getSize();
						if (tree.getSize() > maxTreeSize)
							maxTreeSize = tree.getSize();
						if (graph->getSumOfInversedWeights(node) < minInversedWeight)
							minInversedWeight = graph->getSumOfInversedWeights(node);
						if (graph->getSumOfInversedWeights(node) > maxInversedWeight)
							maxInversedWeight = graph->getSumOfInversedWeights(node);
					}
					else{
						solucao.add(0);
					}
					if (cplex.getValue(W[i]) >= infCut){
						reached++;
					}
					if (valuesW[i] >= infCut){
						counter += graph->getMerge(allowedNodes->getNodeByIndex(i));
					}
				}
				switch (modelNumber){
				case 1: printf(" | Reached without merge %d\t\t\t\tPercentage: %.2f%%\n", reached, ((float)reached / allowedNodes->getSize()) * 100); break;
				case 2: printf(" | Reached without merge %d\t\t\t\tPercentage: %.2f%%\n", reached, ((float)reached / allowedNodes->getSize()) * 100); break;
				case 3: printf(" | Number of selected people: %.0f\t\t\t\tPercentage: %.2f%%\n", solutionValue, ((float)solutionValue / allowedNodes->getSize()) * 100); break;
				case 4: printf(" | Reached without merge %d\t\t\t\tPercentage: %.2f%%\n", reached, ((float)reached / allowedNodes->getSize()) * 100); break;
				default: cout << "Wrong model parameter" << endl;
				}
				printf(" | Reached with merge: %d\t\t\t\tPercentage: %.2f%%\n", counter, ((float)counter / allowedNodes->getSize()) * 100);
				env.out() << "**************************************************************************" << endl;

				ids << "}";

				QueryPerformanceCounter(&t10);

				dataOutput << modelNumber << "; " << parameter << "; " << initialC << "; " << infCut << "; " << time(&t10, &t0, &freq) << "; ";
				dataOutput << time(&t2, &t1, &freq) << "; " << time(&t4, &t3, &freq) << "; " << time(&t6, &t5, &freq) << "; " << time(&t8, &t7, &freq) << "; " << time(&t10, &t9, &freq) << "; ";
				dataOutput << solutionValue << "; " << (float)(sInversedWeight / inSolution) << "; " << minInversedWeight << "; " << maxInversedWeight << "; ";
				dataOutput << (float)(sTreeSize / inSolution) << "; " << minTreeSize << "; " << maxTreeSize << "; " << ((float)counter / allowedNodes->getSize()) * 100 << "; " << counter << "; ";
				dataOutput << branching << "; " << iteration << "; " << hasPreviousSolution << "; " << ids.str() << "; " << endl;
			}
			catch (IloException e){
				cerr << e << endl;
				cout << e.getMessage() << endl;
			}
			catch (exception& e){
				//cerr << e << endl;
				cerr << e.what() << endl;
			}
		}
	} while (repeat && branching && (iteration*5) <= timelimit);
	delete[] generatedColumns;
}

void create(IloEnv env, Graph *graph, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R){
	double control = 0;
	stringstream auxText;
	Funct utilities;
	cout << "> Create:      ";
	for (int u = 0; u < graph->getNumberOfNodes(); u++){
		if (graph->isConnected(u + 1)){
			auxText << "w(" << u + 1 << ")";
			W.add(IloNumVar(env, 0, 1, auxText.str().c_str()));
			auxText.str("");
			auxText << "z(" << u + 1 << ")";
			Z.add(IloBoolVar(env, 0, 1, auxText.str().c_str()));
			auxText.str("");
			int index = allowedNodes->add(u + 1);
			auxText << "r_" << u + 1;
			R.add(IloRange(env, -IloInfinity, W[index], 0, auxText.str().c_str()));
			auxText.str("");

			utilities.load(u, graph->getNumberOfNodes(), &control);
		}
	}
	cout << endl;
}

void build(Graph *graph, string append){

	/*PARAMETERS*/
	float maxInfs[] = { 5, 15, 30, 50, 100 };
	float percentages[] = { 0.05, 0.1, 0.2, 0.3, 0.4 };
	float infCuts[] = { 0.001, 0.005, 0.01, 0.1 };

	IloEnv env;
	IloNumArray solucao(env);
	IloModel model(env);
	Dictionary allowedNodes(graph->getNumberOfNodes());
	IloCplex cplex(model);
	IloNumVarArray W(env);
	IloNumVarArray Z(env);
	IloRangeArray R(env);
	IloRangeArray T(env);
	IloObjective objective(env);
	IloRange infFunction(env, 0, IloInfinity, "inf");
	IloRange localB(env, -IloInfinity, 0, "localB");

	create(env, graph, &allowedNodes, W, Z, R);

	model.add(R);
	model.add(infFunction);
	model.add(objective);
	model.add(localB);

	solucao.add(-1);

	solve(1, graph, &allowedNodes, model, W, Z, R, T, objective, infFunction, localB, 15, 0.001, 100, 30, append, solucao);
	solve(2, graph, &allowedNodes, model, W, Z, R, T, objective, infFunction, localB, 15, 0.001, 100, 5, append, solucao);
	
		
	//solve(int modelNumber, Graph *graph, Dictionary *allowedNodes, IloModel model, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R, IloRangeArray T, IloObjective objective, IloRange infFunction, IloRange localB, float parameter, float infCut, int initialC, int timelimit, string append, IloNumArray solucao, bool branching = false){


	//for (int i = 0; i < 5; i++){
	//	for (int j = 0; j < 5; j++){
	//		solve(1, graph, &allowedNodes, model, cplex, W, Z, R, T, objective, infFunction, localB, percentages[i], infCuts[j], 1000, 30, append, solucao);
	//		solucao.clear();
	//	}
	//}
}

int main(int argc, char **argv){

	Graph graph;

	graph.loadFromFile("lorenza.txt");

	build(&graph, "proximaReuniao.csv");

	utilities.wait("FIM");
	//Graph g;
	//g.loadFromFile("lorenza.txt", "amountReached.txt");
	//g.getPathSizeAtAllNodes(1291667);
	//g.excent(785870);
	//g.print();
	//g.brandes();
	//model002("append.csv", GRAPH, MAXINF, INF, INITIALC, ITERATIONLIMIT, ITERATIONC, TIMELIMIT);
}
