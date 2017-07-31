// GETComp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <ilcplex/ilocplex.h>

#define HI 99999
#define LI -99999

int AAA = 0;
using namespace std;
Funct utilities;

int time(LARGE_INTEGER *t2, LARGE_INTEGER *t1, LARGE_INTEGER *f){
	return (int)(((t2->QuadPart - t1->QuadPart) * 1000) / f->QuadPart);
}

int createVariable(int node, IloEnv env, Graph *graph, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z, IloBoolVarArray Y, IloNumVarArray E, IloRangeArray R, IloRangeArray T, IloExpr objective, int model){
	stringstream auxText;
	int index = -1;
	if (graph->isConnected(node)){
		AAA++;
		auxText << "w(" << node << ")";
		switch (model){
		case 1: W.add(IloNumVar(env, 0, 1, auxText.str().c_str())); break;
		case 2: W.add(IloBoolVar(env, 0, 1, auxText.str().c_str())); break;
		case 3: W.add(IloBoolVar(env, 0, 1, auxText.str().c_str())); break;
		default: cout << "Wrong model parameter" << endl;
		}
		auxText.str("");
		auxText << "z(" << node << ")";
		Z.add(IloBoolVar(env, 0, 1, auxText.str().c_str()));
		auxText.str("");
		auxText << "e(" << node << ")";
		E.add(IloNumVar(env, 0, 1, auxText.str().c_str()));
		auxText.str("");
		index = allowedNodes->add(node);
		switch (model){
		case 1: objective += W[index]; break;
		case 2: objective += W[index]; break;
		case 3: objective += Z[index]; break;
		default: cout << "Wrong model parameter"; break;
		}
		auxText << "r_" << node;
		R.add(IloRange(env, -IloInfinity, W[index], 0, auxText.str().c_str()));
		auxText.str("");
		auxText << "t_" << node;
		T.add(0 <= Z[index] <= 1);
		auxText.str("");
	}
	//cout << "Created variables for " << node << " - index: " << index << endl;
	return index;
}

void createColumn(Graph *graph, bool *generatedColumns, int node, Dictionary *allowedNodes, IloRangeArray R, IloNumVarArray Z, IloNumVarArray W, IloNumVarArray E, IloBoolVarArray Y, IloRangeArray T, IloExpr objective, IloRange infFunction, double cut, int model, bool option = false){
	Tree tree;
	graph->breadthSearch(&tree, node, cut);
	register int index = allowedNodes->getIndexByNode(node);
	//if (index == -1){
	//	index = createVariable(node, R.getEnv(), graph, allowedNodes, W, Z, Y, E, R, T, objective, model);
	//}
	if (generatedColumns[index]){
		cout << "Coluna: " << node << endl;
		utilities.wait("Erro ao gerar colunas");
	}
	generatedColumns[index] = true;
	double control = 0;
	int current;
	register int dIndex;
	for (int f = 0; f < tree.getSize(); f++){
		current = tree.nodes[f];
		dIndex = allowedNodes->getIndexByNode(current);
		//if (dIndex == -1){
		//	dIndex = createVariable(current, R.getEnv(), graph, allowedNodes, W, Z, Y, E, R, T, objective, model);
		//}
		if (option)
			utilities.load(f, tree.getSize(), &control);
		if ((model == 2 || model == 3) && tree.info[f] > 0){
			R[dIndex].setLinearCoef(Z[index], -1);
		}
		if (model == 1 && tree.getInfoByVertex(current) != 0){
			R[dIndex].setLinearCoef(Z[index], (double) -1 * tree.info[f]);
		}
	}
	R[index].setLinearCoef(Z[index], -1);
	switch (model){
	case 1: infFunction.setLinearCoef(Z[index], 1); break;
	case 2:	infFunction.setLinearCoef(Z[index], 1); break;
	case 3:	break;
	}
}

/*void createColumn(Graph *graph, bool *generatedColumns, int node, Dictionary *allowedNodes, IloRangeArray R, IloNumVarArray Z, IloNumVarArray W, IloNumVarArray E, IloBoolVarArray Y, IloRangeArray T, IloExpr objective, IloRange infFunction, double cut, int model, bool option = false){
	Tree tree;
	graph->breadthSearch(&tree, node, cut);
	int index = allowedNodes->getIndexByNode(node);
	if (generatedColumns[index]){
		cout << "Coluna: " << node << endl;
		utilities.wait("Erro ao gerar colunas");		
	}
	generatedColumns[index] = true;
	double control = 0;
	for (int f = 0; f < allowedNodes->getSize(); f++){
		int current = allowedNodes->getNodeByIndex(f);
		if (option)
			utilities.load(f, allowedNodes->getSize(), &control);
		if ((model==2 || model==3) && tree.getInfoByVertex(current) > 0){
			R[f].setLinearCoef(Z[index], -1);
		}
		if (model==1 && tree.getInfoByVertex(current) != 0){
			R[f].setLinearCoef(Z[index], -1 * tree.getInfoByVertex(current));
		}
	}
	R[index].setLinearCoef(Z[index], -1);
	switch (model){
	case 1: infFunction.setLinearCoef(Z[index], 1); break;
	case 2:	infFunction.setLinearCoef(Z[index], 1); break;
	case 3:	//infFunction.setLinearCoef(W[index], 1);
		break;
	}
}*/

/*bool chooseNodeGreedy(Graph *graph, Dictionary *allowedNodes, IloRangeArray R, IloBoolVarArray Z, IloNumVarArray W, IloNumArray valuesZ, IloNumArray valuesW, bool *generatedColumns, IloRange fI, double cut, int iterationC, int iterationLimit, float alpha, int model, bool ops = true){
	cout << "> Choosing " << iterationC << " new columns: " << endl;
	MaxHeap heap(graph->getNumberOfNodes());
	list<int> sorted;
	for(int i = 0; i < allowedNodes->getSize(); i++){
		if(!generatedColumns[i]){
			int node = allowedNodes->getNodeByIndex(i);
			heap.add(node, graph->getSumOfInversedWeights(node) * (1 - valuesW[i]) );
		}
	}
	heap.sort(&sorted);
	cout << endl;
	srand(heap.size());
	int control = (int) alpha * graph->getNumberOfNodes();
	int iteration = 0;
	while(iteration < control && !sorted.empty()){
		sorted.pop_front();
		iteration++;
	}
	if (sorted.empty())
		return false;
	iteration = 0;
	//while(iteration < iterationC && !sorted.empty()){
		cout << "| Creating from " << sorted.front() << " :      ";
		if (ops)
			createColumn(graph, generatedColumns, sorted.front(), allowedNodes, R, Z, fI, cut, model, true);
		else
			createColumn2(graph, generatedColumns, sorted.front(), allowedNodes, R, Z, W, fI, cut, model, true);
		sorted.pop_front();
		iteration++;
	//}
	cout << "> Going back to master problem" << endl << endl;
	return true;
}*/

/*bool chooseNode(Graph *graph, Dictionary *allowedNodes, IloRangeArray R, IloBoolVarArray Z, IloBoolVarArray W, IloNumArray valuesZ, IloNumArray valuesW, bool *generatedColumns, IloRange fI, double cut, int iterationC, int iterationLimit, int model){
	cout << "> Choosing " << iterationC << " new columns: " << endl;
	int sorted;
	int counter = 0;
	double control = 0;
	int iteration = 0;
	srand(iterationLimit);
	node *nodes = new node[allowedNodes->getSize()];
	for (int i = 0; i < allowedNodes->getSize(); i++){
		nodes[i].id = allowedNodes->getNodeByIndex(i);
		nodes[i].chave = (1 - valuesW[i])*graph->getSumOfInversedWeights(nodes[i].id);
	}
	heapSorti(nodes, allowedNodes->getSize());
	for (int i = 0; i < allowedNodes->getSize() && counter < iterationC; i++){
		int node = nodes[i].id;
		sorted = allowedNodes->getIndexByNode(node);
		if (!generatedColumns[sorted] && nodes[i].chave > cut){
			cout << "| Creating from " << node << " :      ";
			createColumn(graph, generatedColumns, node, allowedNodes, R, Z, W, fI, cut, model, true);
			cout << endl;
			counter++;
		}
	}
	if (counter == 0){
		cout << "> Out of great nodes" << endl;
		return false;
	}
	cout << "> Going back to master problem" << endl << endl;
	return true;
}*/

void populate(IloEnv env, Graph *graph, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z, IloBoolVarArray Y, IloNumVarArray E, IloRangeArray R, IloRangeArray T,IloExpr objective, int model){
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
			default: cout << "Wrong model parameter" << endl;
			}
			auxText.str("");
			auxText << "z(" << u + 1 << ")";
			Z.add(IloBoolVar(env, 0, 1, auxText.str().c_str()));
			auxText.str("");
			auxText << "e(" << u + 1 << ")";
			E.add(IloNumVar(env, 0, 1, auxText.str().c_str()));
			auxText.str("");
			int index = allowedNodes->add(u + 1);
			switch (model){
			case 1: objective += W[index]; break;
			case 2: objective += W[index]; break;
			case 3: objective += Z[index]; break;
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

void createInitialColumns(IloEnv env, Graph *graph, bool *generatedColumns, Dictionary *allowedNodes, IloRangeArray R, IloNumVarArray Z, IloNumVarArray W, IloNumVarArray E, IloBoolVarArray Y, IloRangeArray T, IloExpr objective, IloRange funcaoInf, double cut, int amount, int orderingBy, int model){
	list<int> allowedZ;
	cout << "> Choosing first nodes to generate columns:      ";
	graph->getInitialVertexes(&allowedZ, orderingBy, amount);
	cout << endl;
	cout << "> Creating columns:      ";
	double control = 0;
	int initialSize = (int) allowedZ.size();
	bool first = false;
	while (!allowedZ.empty()){
		createColumn(graph, generatedColumns, allowedZ.front(), allowedNodes, R, Z, W, E, Y, T, objective, funcaoInf, cut, model, first);
		allowedZ.pop_front();
		utilities.load(initialSize - (int) allowedZ.size(), initialSize, &control);
	}
	cout << endl << endl;
}

void exportInfo(int k, IloCplex cplex, IloNumArray valuesW, IloNumArray valuesE, IloBoolVarArray Z, Dictionary *allowedNodes, bool *generatedColumns, double cut){
	IloEnv env = cplex.getEnv();
	stringstream auxText;

	auxText << "model_k_" << k << ".lp";
	cplex.exportModel(auxText.str().c_str());
	auxText.str("");

	/*auxText << "values_k_" << k << ".csv";
	ofstream arq(auxText.str().c_str());
	auxText.str("");
	arq << "Node ; Z value; W value ; E value;" << endl;
	for (int u = 0; u < allowedNodes->getSize(); u++){
		if (valuesW[u] >= cut){
			int n = allowedNodes->getNodeByIndex(u);
			arq << n << "; ";
			if (generatedColumns[u])
				arq << cplex.getValue(Z[u]) << "; ";
			else
				arq << " 0; ";
			arq << valuesW[u] << "; " << valuesE[u] << "; " << endl;
		}
	}*/
}

void showStatistics(Graph *graph, Dictionary *allowedNodes, bool *generatedColumns, IloCplex cplex, IloBoolVarArray Z, int maxInf, double cut){
	int *counter = new int[graph->getNumberOfNodes()];
	utilities.setInt(counter, graph->getNumberOfNodes(), 0);
	Tree **trees = new Tree*[maxInf];
	for (int u = 0; u < maxInf; u++){
		trees[u] = NULL;
	}
	int indexController = 0;
	int biggest = 0;
	IloEnv env = cplex.getEnv();
	for (int i = 0; i < allowedNodes->getSize(); i++){
		int n = allowedNodes->getNodeByIndex(i);
		if (generatedColumns[i] && cplex.getValue(Z[i]) > 0.9){
			env.out() << "[" << n << "]: " << endl;
			env.out() << " | Soma dos pesos dos arcos que chegam : " << graph->getSumOfInversedWeights(n) << endl;
			trees[indexController] = new Tree;
			graph->breadthSearch(trees[indexController], n, cut);
			cout << " | Tamanho da arvore de busca: " << trees[indexController]->getSize() << endl;
			indexController++;
		}
	}
	for (int u = 0; u < graph->getNumberOfNodes(); u++){
		for (int c = 0; trees[c] != NULL && c < maxInf; c++){
			if (trees[c]->getInfoByVertex(u + 1) >= cut){
				counter[u]++;
			}
			if (counter[u] > biggest){
				biggest = counter[u];
			}
		}
	}
	int *statics = new int[biggest + 1];
	utilities.setInt(statics, biggest + 1, 0);
	for (int u = 0; u < graph->getNumberOfNodes(); u++){
		statics[counter[u]]++;
	}
	cout << endl;
	cout << " > Analysis of selected nodes" << endl;
	for (int u = 1; u < biggest + 1; u++){
		if (statics[u] != 0){
			cout << "| " << statics[u] << " node(s) are reached by " << u << " of selected nodes" << endl;
		}
	}
	cout << endl;
	delete[] statics;
	delete[] counter;
	for (int u = 0; u < maxInf; u++)
		delete trees[u];
	trees = NULL;
}

void parameters(IloCplex cplex, int initialC, int initialR, int timelimit){
	cplex.setParam(IloCplex::TiLim, timelimit* 60);
	cplex.setParam(IloCplex::MemoryEmphasis, true);
	cplex.setParam(IloCplex::WorkMem, 100);
	cplex.setParam(IloCplex::MIPEmphasis, 2);
	cplex.setParam(IloCplex::NodeFileInd, 1);
	cplex.setParam(IloCplex::ColReadLim, initialC);
	cplex.setParam(IloCplex::RowReadLim, initialR);
	cplex.setParam(IloCplex::NumericalEmphasis, 1);
	cout <<  "**************************************************************************" << endl;
	cout << " | Time limit: " << cplex.getParam(IloCplex::TiLim)  << " seconds" << endl;
	cout << " | Memory emphasis: " << cplex.getParam(IloCplex::MemoryEmphasis) << endl;
	cout << " | Workable memory: " << cplex.getParam(IloCplex::WorkMem) << " MBs" << endl;
	cout << " | MIP Emphashis: " << cplex.getParam(IloCplex::MIPEmphasis) << endl;
	cout << " | NodeFileInd: " << cplex.getParam(IloCplex::NodeFileInd) << endl;
	cout << " | ColReadLim: " << cplex.getParam(IloCplex::ColReadLim) << endl;
	cout << " | RowReadLim: " << cplex.getParam(IloCplex::RowReadLim) << endl;
	cout << "**************************************************************************" << endl;
}

// Z binário, W fracionário, maximiza quantidade de informação recebida (somatório de W) atendendo no máximo maxInf pessoas
void model001(string append, Graph *graph, int maxInf, double infCut, int initialC, int iterationLimit, int iterationC, int timelimit, IloNumArray sol, int orderingBy = 1){
	//int *solution = new int[maxInf];
	ofstream app(append.c_str(), ios::app);
	LARGE_INTEGER t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, freq;
	QueryPerformanceCounter(&t0);
	QueryPerformanceFrequency(&freq);


	QueryPerformanceCounter(&t1);
	//Graph graph;
	//graph->loadFromFile(path, "amountReachedAllNodes.txt");
	QueryPerformanceCounter(&t2);

	IloEnv env;
	Dictionary allowedNodes(graph->getNumberOfNodes());
	stringstream auxText;
	IloModel model(env);
	IloCplex cplex(model);
	IloNumVarArray W(env);
	IloNumVarArray E(env);
	IloNumVarArray Z(env);
	IloBoolVarArray L(env);
	IloRangeArray R(env);
	IloRangeArray T(env);
	IloExpr maximize(env);
	IloRange funcaoInf(env, -IloInfinity, 0, "inf");


	QueryPerformanceCounter(&t3);
	populate(env, graph, &allowedNodes, W, Z, L, E, R, T, maximize, 1);
	QueryPerformanceCounter(&t4);

	int sGeneratedColumns = graph->getNumberOfNodes() - graph->getNumberOfDisconnected();
	bool *generatedColumns = new bool[sGeneratedColumns];
	utilities.setBool(generatedColumns, sGeneratedColumns, false);

	QueryPerformanceCounter(&t5);
	createInitialColumns(env, graph, generatedColumns, &allowedNodes, R, Z, W, E, L, T, maximize, funcaoInf, infCut, initialC, orderingBy, 1);
	QueryPerformanceCounter(&t6);

	funcaoInf.setUb(maxInf);
	model.add(IloMaximize(env, maximize));
	model.add(funcaoInf);
	model.add(R);
	model.add(T);

	int iteration = 0;
	bool repeat = false;

	IloNumArray valuesW(env);
	IloNumArray valuesZ(env);
	IloNumArray valuesE(env);

	do{
		int sTreeSize = 0, minTreeSize = HI, maxTreeSize = LI;
		float sInversedWeight = 0, minInversedWeight = HI, maxInversedWeight = LI;
		iteration++;
		QueryPerformanceCounter(&t7);
		parameters(cplex, initialC, allowedNodes.getSize(), timelimit);
		try{
			if (sol[0] != -1){
				cplex.addMIPStart(Z, sol, IloCplex::MIPStartAuto, "prevSol");
			}
		}
		catch (IloException& e){
			cerr << e.getMessage() << endl;
		}
		catch (exception& e){
			cerr << e.what() << endl;
		}
		if (!cplex.solve()) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		QueryPerformanceCounter(&t8);

		cplex.exportModel("model001.lp");
		//cplex.getValues(Z, sol);
		sol.clear();
		cplex.getValues(W, valuesW);
		try{
		}
		catch (IloException& e){
			cerr << e.getMessage() << endl;
		}
		IloNum solutionValue = cplex.getObjValue();
		int solutionIndex = 0;
		int reached = 0;
		int inSolution = 0;
		env.out() << endl << "**************************************************************************" << endl;
		env.out() << " | MODEL 001" << endl;
		env.out() << " | maxInf: " << maxInf << "\tSolution status = " << cplex.getStatus() << "\tSolution value = " << solutionValue << endl;
		env.out() << " > People who received original information:" << endl;
		for (int i = 0; i < allowedNodes.getSize(); i++){
			int node = allowedNodes.getNodeByIndex(i);
			if (generatedColumns[i] && cplex.getValue(Z[i]) > 0.9){
				//solution[solutionIndex++] = node;
				sol.add(1);
				inSolution++;
				Tree tree;
				graph->breadthSearch(&tree, node, infCut);
				env.out() << " | ID:  " << node << "\tSumOfInversedWeights: " << graph->getSumOfInversedWeights(node) << "\tTree size:  " << tree.getSize() << endl;
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
				sol.add(0);
			}
			if (cplex.getValue(W[i]) >= infCut){
				reached++;
			}
		}

		int counter = 0;
		for (int u = 0; u < allowedNodes.getSize(); u++){
			if (valuesW[u] >= infCut){
				counter += graph->getMerge(allowedNodes.getNodeByIndex(u));
			}
		}

		printf(" | Reached without merge %d\t\t\t\tPercentage: %.2f%%\n", reached, ((float)reached / allowedNodes.getSize()) * 100);
		printf(" | Reached with merge: %d\t\t\t\tPercentage: %.2f%%\n", counter, ((float)counter / allowedNodes.getSize()) * 100);

		env.out() << "**************************************************************************" << endl;
		QueryPerformanceCounter(&t9);

		repeat = false; //chooseNode(graph, &allowedNodes, R, Z, W, valuesZ, valuesW, generatedColumns, funcaoInf, infCut, iterationC, iterationLimit, 2);
		QueryPerformanceCounter(&t10);
		app << "1 ; " << maxInf << "; " << initialC << "; " << infCut << "; " << time(&t10, &t0, &freq) << "; ";
		app << time(&t4, &t3, &freq) << "; " << time(&t6, &t5, &freq) << "; ";
		app << time(&t8, &t7, &freq) << "; " << solutionValue << "; " << (float)(sInversedWeight / inSolution) << "; " << minInversedWeight << "; " << maxInversedWeight << "; ";
		app << (float)(sTreeSize / inSolution) << "; " << minTreeSize << "; " << maxTreeSize << "; " << ((float)counter / allowedNodes.getSize()) * 100 << "; " << counter << "; " << endl;
		
	} while (repeat);
	env.end();
	delete[] generatedColumns;
}

// Z binário, W binário, maximiza número de pessoas alcançadas (somatório de W) atendendo no máximo maxInf pessoas
void model002(string append, Graph *graph, int maxInf, double infCut, int initialC, int iterationLimit, int iterationC, int timelimit, IloNumArray sol, int orderingBy = 1) {
	//int *solution = new int[maxInf];
	ofstream app(append.c_str(), ios::app);
	LARGE_INTEGER t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, freq;
	QueryPerformanceCounter(&t0);
	QueryPerformanceFrequency(&freq);
	
	IloEnv env;
	//Graph graph;

	QueryPerformanceCounter(&t1);
	//graph->loadFromFile(path, "amountReachedAllNodes.txt");
	QueryPerformanceCounter(&t2);

	Dictionary allowedNodes(graph->getNumberOfNodes());
	stringstream auxText;
	IloModel model(env);
	IloCplex cplex(model);
	IloNumVarArray W(env);
	IloNumVarArray E(env);
	IloNumVarArray Z(env);
	IloBoolVarArray L(env);
	IloRangeArray R(env);
	IloRangeArray T(env);
	IloExpr maximize(env);
	IloRange funcaoInf(env, -IloInfinity, 0, "inf");

	QueryPerformanceCounter(&t3);
	populate(env, graph, &allowedNodes, W, Z, L, E, R, T, maximize, 2);
	QueryPerformanceCounter(&t4);

	int sGeneratedColumns = graph->getNumberOfNodes() - graph->getNumberOfDisconnected();
	bool *generatedColumns = new bool[sGeneratedColumns];
	utilities.setBool(generatedColumns, sGeneratedColumns, false);

	QueryPerformanceCounter(&t5);
	createInitialColumns(env, graph, generatedColumns, &allowedNodes, R, Z, W, E, L, T, maximize, funcaoInf, infCut, initialC, orderingBy, 2);
	QueryPerformanceCounter(&t6);

	funcaoInf.setUb(maxInf);
	model.add(IloMaximize(env, maximize));
	model.add(funcaoInf);
	model.add(R);
	model.add(T);
	
	int iteration = 0;
	bool repeat = false, stop = true;
	
	IloNumArray valuesW(env);
	IloNumArray valuesZ(env);
	IloNumArray valuesE(env);
	
	do{
		int sTreeSize = 0, minTreeSize = HI, maxTreeSize = LI;
		float sInversedWeight = 0, minInversedWeight = HI, maxInversedWeight = LI;
		iteration++;
		QueryPerformanceCounter(&t7);
		parameters(cplex, initialC, allowedNodes.getSize(), timelimit);

		try{
			if (sol[0]!=-1)
				cplex.addMIPStart(Z, sol, IloCplex::MIPStartAuto, "prevSol");
		}
		catch (IloException& e){
			cerr << e.getMessage() << endl;
		}
		
		if (!cplex.solve()) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		QueryPerformanceCounter(&t8);

		cplex.exportModel("model002.lp");
		cplex.getValues(W, valuesW);
		//cplex.getValues(Z, sol);
		sol.clear();

		IloNum solutionValue = cplex.getObjValue();
		int solutionIndex = 0;
		int reached = 0;
		int inSolution = 0;
		env.out() << endl << "**************************************************************************" << endl;
		env.out() << " | MODEL 002" << endl;
		env.out() << " | maxInf: " << maxInf << "\tSolution status = " << cplex.getStatus() << "\tSolution value = " << solutionValue << endl;
		env.out() << " > People who received original information:" << endl;
		for (int i = 0; i < allowedNodes.getSize(); i++){
			int node = allowedNodes.getNodeByIndex(i);
			if (generatedColumns[i] && cplex.getValue(Z[i]) > 0.9){
				//solution[solutionIndex++] = node;
				sol.add(1);
				inSolution++;
				Tree tree;
				graph->breadthSearch(&tree, node, infCut);
				env.out() << " | ID:  " << node << "\tSumOfInversedWeights: " << graph->getSumOfInversedWeights(node) << "\tTree size:  " << tree.getSize() << endl;
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
				sol.add(0);
			}
			if (cplex.getValue(W[i]) >= infCut){
				reached++;
			}
		}

		int counter = 0;
		for (int u = 0; u < allowedNodes.getSize(); u++){
			if (valuesW[u] >= infCut){
				counter += graph->getMerge(allowedNodes.getNodeByIndex(u));
			}
		}

		printf(" | Reached without merge %d\t\t\t\tPercentage: %.2f%%\n", reached, ((float)reached / allowedNodes.getSize()) * 100);
		printf(" | Reached with merge: %d\t\t\t\tPercentage: %.2f%%\n", counter, ((float)counter / allowedNodes.getSize()) * 100);

		env.out() << "**************************************************************************" << endl;
		QueryPerformanceCounter(&t9);

		repeat = false; //chooseNode(graph, &allowedNodes, R, Z, W, valuesZ, valuesW, generatedColumns, funcaoInf, infCut, iterationC, iterationLimit, 2);
		QueryPerformanceCounter(&t10);
		app << "2 ;" << maxInf << "; " << initialC << "; " << infCut << "; " << time(&t10, &t0, &freq) << "; ";
		app << time(&t4, &t3, &freq) << "; " << time(&t6, &t5, &freq) << "; ";
		app << time(&t8, &t7, &freq) << "; " << solutionValue << "; " << (float) (sInversedWeight/inSolution) << "; " << minInversedWeight << "; " << maxInversedWeight << "; ";
		app << (float)(sTreeSize / inSolution) << "; " << minTreeSize << "; " << maxTreeSize << "; " << ((float)counter / allowedNodes.getSize()) * 100 << "; " << counter << "; " << endl;
	} while (repeat);
	delete[] generatedColumns;
	env.end();
}

// Z binário, W binário, minimiza número de pessoas escolhidas inicialmente (somatório de Z) atendendo no mínimo uma porcentagem da rede
void model003(string append, Graph *graph, float percentage, double infCut, int initialC, int iterationLimit, int iterationC, int timelimit, IloNumArray sol, int orderingBy = 1){

	ofstream app(append.c_str(), ios::app);
	LARGE_INTEGER t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, freq;
	QueryPerformanceCounter(&t0);
	QueryPerformanceFrequency(&freq);

	IloEnv env;

	QueryPerformanceCounter(&t1);
	//Graph graph;
	//graph->loadFromFile(path, "amountReachedAllNodes.txt");
	QueryPerformanceCounter(&t2);

	Dictionary allowedNodes(graph->getNumberOfNodes());
	stringstream auxText;
	IloModel model(env);
	IloCplex cplex(model);
	IloNumVarArray W(env);
	IloNumVarArray E(env);
	IloNumVarArray Z(env);
	IloBoolVarArray L(env);
	IloRangeArray R(env);
	IloRangeArray T(env);
	IloExpr minimize(env);
	IloRange funcaoInf(env, 0, IloInfinity, "inf");

	QueryPerformanceCounter(&t3);
	populate(env, graph, &allowedNodes, W, Z, L, E, R, T, minimize, 3);
	QueryPerformanceCounter(&t4);

	int sGeneratedColumns = graph->getNumberOfNodes() - graph->getNumberOfDisconnected();
	bool *generatedColumns = new bool[sGeneratedColumns];
	utilities.setBool(generatedColumns, sGeneratedColumns, false);

	QueryPerformanceCounter(&t5);
	createInitialColumns(env, graph, generatedColumns, &allowedNodes, R, Z, W, E, L, T, minimize, funcaoInf, infCut, initialC, orderingBy, 3);
	QueryPerformanceCounter(&t6);

	model.add(IloMinimize(env, minimize));

	//variável de excesso
	funcaoInf.setLb(allowedNodes.getSize()*percentage);
	funcaoInf.setExpr(IloSum(W));

	model.add(funcaoInf);
	model.add(R);

	int iteration = 0;
	bool repeat = false, stop = true;

	IloNumArray valuesW(env);
	IloNumArray valuesZ(env);
	IloNumArray valuesE(env);

	do{
		int sTreeSize = 0, minTreeSize = HI, maxTreeSize = LI;
		float sInversedWeight = 0, minInversedWeight = HI, maxInversedWeight = LI;
		iteration++;

		parameters(cplex, initialC, allowedNodes.getSize(), timelimit);
		try{
			if (sol[0] != -1){
				cplex.addMIPStart(Z, sol, IloCplex::MIPStartAuto, "prevSol");
			}
		}
		catch (IloException& e){
			cerr << e.getMessage() << endl;
		}
		catch (exception& e){
			cerr << e.what() << endl;
		}

		QueryPerformanceCounter(&t7);
		if (!cplex.solve()) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		QueryPerformanceCounter(&t8);

		cplex.exportModel("model003.lp");
		cplex.getValues(W, valuesW);
		//cplex.getValues(Z, sol);
		sol.clear();

		IloNum solutionValue = cplex.getObjValue();
		int inSolution = 0;
		env.out() << endl << "**************************************************************************" << endl;
		env.out() << " | MODEL 003" << endl;
		env.out() << " | Percentage: " << percentage*100 << "%\tSolution status = " << cplex.getStatus() << "\tSolution value = " << solutionValue << endl;
		env.out() << " > People who received original information:" << endl;
		for (int i = 0; i < allowedNodes.getSize(); i++){
			int node = allowedNodes.getNodeByIndex(i);
			if (generatedColumns[i] && cplex.getValue(Z[i]) > 0.9){
				inSolution++;
				sol.add(1);
				Tree tree;
				graph->breadthSearch(&tree, node, infCut);
				env.out() << " | ID:  " << node << "\tSumOfInversedWeights: " << graph->getSumOfInversedWeights(node) << "\tTree size:  " << tree.getSize() << endl;
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
				sol.add(0);
			}
		}
		int counter = 0;
		for (int u = 0; u < allowedNodes.getSize(); u++){
			if (valuesW[u] >= infCut){
				counter += graph->getMerge(allowedNodes.getNodeByIndex(u));
			}
		}
		printf(" | Number of selected people: %.0f\t\t\t\tPercentage: %.2f%%\n", solutionValue, ((float)solutionValue / allowedNodes.getSize()) * 100);
		printf(" | Reached with merge: %d\t\t\t\tPercentage: %.2f%%\n", counter, ((float)counter / allowedNodes.getSize()) * 100);
		env.out() << "**************************************************************************" << endl;
		QueryPerformanceCounter(&t9);
		repeat = false;//chooseNode(graph, &allowedNodes, R, Z, W, valuesZ, valuesW, generatedColumns, funcaoInf, infCut, iterationC, iterationLimit, 3);
		QueryPerformanceCounter(&t10);
		app << "3 ; " << percentage*100 << "; " << initialC << "; " << infCut << "; " << time(&t10, &t0, &freq) << "; ";
		app << time(&t4, &t3, &freq) << "; " << time(&t6, &t5, &freq) << "; ";
		app << time(&t8, &t7, &freq) << "; " << solutionValue << "; " << (float)(sInversedWeight / inSolution) << "; " << minInversedWeight << "; " << maxInversedWeight << "; ";
		app << (float)(sTreeSize / inSolution) << "; " << minTreeSize << "; " << maxTreeSize << "; " << ((float)counter / allowedNodes.getSize()) * 100 << "; " << counter << "; " << endl;
	} while (repeat);
	delete[] generatedColumns;
	env.end();
}

int main(int argc, char **argv){

	Graph graph;
	IloEnv envr;
	
	//graph.loadFromFile("ale.txt", "aa.txt");
	//IloNumArray solution(envr);
	//solution.add(IloNum(-1));
	//cout << solution << endl;
	//model001("append.csv", &graph, 15, 0.001, 10000, 50, 10, 30, solution, 1);
	//cout << "FINISHED MODEL 001" << endl;
	//model002("append.csv", &graph, 15, 0.001, 10000, 50, 10, 30, solution, 1);
	//cout << "FINISHED MODEL 002 " << endl;
	//model002("append.csv", &graph, 15, 0.001, 10000, 50, 10, 60, solution, 1);
	//cout << "FINISHED MODEL 002 " << endl;
	//model002("append.csv", &graph, 15, 0.001, 10000, 50, 10, 90, solution, 1);
	//cout << "FINISHED MODEL 002 " << endl;
	//model003("append.csv", &graph, 0.5, 0.001, 10000, 50, 10, 120, solution, 1); 
	//cout << "FINISHED MODEL 003" << endl;
	//model003("append.csv", &graph, 0.5, 0.001, 10000, 50, 10, 30, solution, 1);
	//cout << "FINISHED MODEL 003" << endl;
	//model003("append.csv", &graph, 0.5, 0.001, 10000, 50, 10, 60, solution, 1);
	//cout << "FINISHED MODEL 003" << endl;
	//model003("append.csv", &graph, 0.5, 0.001, 10000, 50, 10, 90, solution, 1);
	//cout << "FINISHED MODEL 003" << endl;
	//model003("append.csv", &graph, 0.5, 0.001, 10000, 50, 10, 120, solution, 1);
	//cout << "FINISHED MODEL 003" << endl;

	graph.loadFromFile("ale.txt", "aa.txt");
	IloNumArray solution(envr);
	solution.add(IloNum(-1));
	cout << solution << endl;
	model001("app.csv", &graph, 2, 0.5, 2, 20, 1, 200, solution, true);
	cout << "FINISHED MODEL 001" << endl;
	model002("app.csv", &graph, 2, 0.5, 1, 20, 1, 200, solution, true);
	cout << "FINISHED MODEL 002" << endl;
	model003("app.csv", &graph, 0.9, 0.5, 2, 20, 1, 200, solution, true);
	cout << "FINISHED MODEL 003" << endl;

	utilities.wait("FIM");
	//Graph g;
	//g.loadFromFile("lorenza.txt", "amountReached.txt");
	//g.getPathSizeAtAllNodes(1291667);
	//g.excent(785870);
	//g.print();
	//g.brandes();
	//model002("append.csv", GRAPH, MAXINF, INF, INITIALC, ITERATIONLIMIT, ITERATIONC, TIMELIMIT);
}
