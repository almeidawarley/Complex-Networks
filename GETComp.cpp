/**
	Information Diffusion in Complex Networks
	GETComp.cpp
	Purpose: contains the main function of the project

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"
#include <ilcplex/ilocplex.h>
#include <windows.h>
#include <vector>
#include <time.h>
#define HI +99999 // positive infinite
#define LI -99999 // negative infinite

using namespace std;

int previousNColumns = -1;
int previousModel = -1;
int previousCoefType = -1;
double previousInfCut = -1;

IloEnv env;
Funct ut;
LARGE_INTEGER t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, freq;
bool *ignored = NULL;
bool comparison = true;
stringstream ssIgnored;
vector<int> columnsIDs;

ostream& operator<<(ostream& os, const vector<int>& elements){
	os << "{";
	for (int i = 0; i < elements.size(); i++){
		os << elements[i];
		if(i != elements.size()-1) 
			os << ";";
	}
	os << "}";
	return os;
}

/*
	Checks if a node is ignored in a specific run of the algorithm
	*@param int node: node to be checked
	*@return bool: true if a node is ignored, false if it is not ignored
*********************************************************/
bool isIgnored(int node){
	return ignored[node - 1];
}

/*
	Frees the ignored array and set its value to NULL
	*@param -
	*@return void: -
*********************************************************/
void freeIgnored(){
	delete[] ignored;
	ignored = NULL;
}

/*
	Defines the coefficient type based on a model number
	*@param int model:	model number
	*@return int:		coefficient type
*********************************************************/
int defineCoefType(int model){
	if (model == 1 || model == 4)
		return 0;
	else
		return 1;
}

/*
	Checks whether the model is the same from previous run
	*@param int coefType:	model number
			double infCut:	information cut parameter
			int nColumns:	generated number of columns
	*@return bool:			true if new columns do not need to be generated, false if they do
*********************************************************/
bool checkCoefType(int coefType, double infCut, int nColumns){
	bool answer = false;
	if (coefType == previousCoefType && infCut == previousInfCut && nColumns == previousNColumns)
		answer = true;
	previousCoefType = coefType;
	previousInfCut = infCut;
	previousNColumns = nColumns;
	return answer;
}

/*
	Calculates the amount of time between two diferent time marks
	*@param LARGE_INTEGER t1: indicates the first time mark
			LARGE_INTEGER t2: indicates the second time mark
			LARGE_INTEGER f: indicates the frequency of the processor
	*@return int: the amount of time between t2 and t1
*********************************************************/
int time(LARGE_INTEGER *t2, LARGE_INTEGER *t1, LARGE_INTEGER *f){
	return (int)(((t2->QuadPart - t1->QuadPart) * 1000) / f->QuadPart);
}

/*
	Generates the column of a specific node 
	*@param Graph *graph:				is a pointer to the graph
			bool *generatedColumns:		generatedColumns[u] indicates whether a column for node u was generated or not
			int node:					stores the node that will generate the new column
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			IloRangeArray R:			is an array of constraints following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloNumVarArray W:			is an array with variables W following the model
			double cut:					determines when the information diffusion tree will be cut
			int model:					indicates which model is used in this run
			bool option:				indicates whether the percentage amount will be shown
	*@return void: -
*********************************************************/
void createColumn(Graph *graph, bool *generatedColumns, int node, Dictionary *allowedNodes, IloRangeArray R, IloNumVarArray Z, IloNumVarArray W, double cut, int model, bool option = false){
	Tree tree;
	graph->breadthSearch(&tree, node, cut);
	register int index = allowedNodes->getIndexByNode(node);
	if (generatedColumns[index]){
		cout << "Coluna: " << node << " | Index: " << index << endl;
		ut.wait("Erro ao gerar colunas");
	}
	if (ignored != NULL && isIgnored(node) == comparison){
		cout << "Node " << node << " ignored due to " << (comparison ? "high" : "low") << " similarity" << endl;
		return;
	}
	generatedColumns[index] = true;
	double control = 0;
	int current;
	register int dIndex;
	for (int f = 0; f < tree.getSize(); f++){
		current = tree.nodes[f];
		dIndex = allowedNodes->getIndexByNode(current);
		if (option)
			ut.load(f, tree.getSize(), &control);
		if ((model == 2 || model == 3 || model == 5) && tree.info[f] > 0){
			R[dIndex].setLinearCoef(Z[index], -1);
		}
		if ((model == 1 || model == 4) && tree.getInfoByVertex(current) != 0){
			R[dIndex].setLinearCoef(Z[index], (double) -1 * tree.info[f]);
		}
	}
	R[index].setLinearCoef(Z[index], -1);
}

/*
	Populates the arrays R, Z and W based on the network's information
	*@param Graph *graph:				is a pointer to the graph
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			IloRangeArray R:			is an array of constraints following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloNumVarArray W:			is an array with variables W following the model
	*@return void: -
*********************************************************/
void build(Graph *graph, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R, IloRangeArray T){
	double control = 0;
	stringstream auxText;
	Funct utilities;
	register int index;
	cout << " > Building:      ";
	for (int u = 0; u < graph->getNumberOfNodes(); u++){
		if (graph->isConnected(u + 1)){
			index = allowedNodes->add(u + 1);
			auxText << "w(" << u + 1 << ")";
			W.add(IloNumVar(env, 0, 1, auxText.str().c_str()));
			auxText.str("");
			auxText << "z(" << u + 1 << ")";
			Z.add(IloBoolVar(env, 0, 1, auxText.str().c_str()));
			auxText.str("");
			auxText << "r_" << u + 1;
			R.add(IloRange(env, -IloInfinity, W[index], 0, auxText.str().c_str()));
			auxText.str(""); 
			auxText << "t_" << u + 1;
			T.add(IloRange(env, -IloInfinity, Z[index], 0, auxText.str().c_str()));
			auxText.str("");
			ut.load(u, graph->getNumberOfNodes(), &control);
		}
	}
	cout << endl;
}

/*
	Generates the columns of the run based on some parameters
	*@param Graph *graph:				is a pointer to the graph
			bool *generatedColumns:		generatedColumns[u] indicates whether a column for node u was generated or not
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			IloRangeArray R:			is an array with variables R following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloNumVarArray W:			is an array with variables W following the model
			double cut:					determines when the information diffusion tree will be cut
			int amount:					indicates the number of columns that will be generated
			int orderingBy:				indicates the criteria of sorting
			int model:					indicates which model is used in this run
	*@return void: -
*********************************************************/
void columns(Graph *graph, bool *generatedColumns, Dictionary *allowedNodes, IloRangeArray R, IloNumVarArray Z, IloNumVarArray W, double cut, int amount, int orderingBy, int model){
	vector<int> allowedZ;
	graph->getInitialNodes(&allowedZ, allowedNodes, orderingBy, amount);	
	int coefType = defineCoefType(model);
	if (!checkCoefType(coefType, cut, amount)){
		columnsIDs.clear();
		for (int i = 0; i < R.getSize(); i++)
			R[i].setExpr(W[i]);

		cout << " > Columns:      ";
		double control = 0;
		int counter = 0;
		for (int z : allowedZ){
			createColumn(graph, generatedColumns, z, allowedNodes, R, Z, W, cut, model, false);
			columnsIDs.push_back(z);
			counter += 1;
			ut.load(counter, (int)allowedZ.size(), &control);
		}
		cout << endl;
	}else{
		cout << " > Columns: Previous model with same coefficient type, no need for new columns" << endl;		
	}
}

/*
	Sets the CPLEX parameters
	*@param IloCplex cplex:				indicates the current CPLEX solver
			int initialC:				indicates the number of initial columns
			int initialR:				indicates the number of constraints
			int timelimit:				indicates in minutes the time limit
			int model:					indicates which model is used in this run
	*@return void: -
*********************************************************/
void parameters(IloCplex cplex, int initialC, int initialR, int timelimit, int model){
	cplex.setParam(IloCplex::TiLim, timelimit * 60);
	cplex.setParam(IloCplex::MemoryEmphasis, true);
	cplex.setParam(IloCplex::WorkMem, 100);
	cplex.setParam(IloCplex::NodeFileInd, 3);
	cplex.setParam(IloCplex::ColReadLim, initialC);
	cplex.setParam(IloCplex::RowReadLim, initialR);
	cplex.setParam(IloCplex::NumericalEmphasis, 1);
	cplex.setParam(IloCplex::PolishAfterDetTime, 2 * 30 * 60);
	cplex.setParam(IloCplex::RINSHeur, 30);
	cplex.setParam(IloCplex::LBHeur, 1);
}

void makeSolutionConstraint(IloRangeArray T, IloNumVarArray Z, IloNumArray solution){
	if (solution.getSize() != 0){
		for (int i = 0; i < solution.getSize(); i++){
			if (solution[i] == 0.9){
				T[i].setExpr(Z[i] == 1);
			}
		}
	}
}

/*
	Generates the constraints and objective function for model 001.

	In this model, Z is a binary variable and W is a real variable.
	It aims to maximize the amount of received information by members of the network (sum of W).
	It is constrained by the number of people that receives the initial information -> only maxInf nodes.
	Portuguese: Z binário, W fracionário, maximiza quantidade de informação recebida (somatório de W) atendendo no máximo maxInf pessoas

	*@param IloModel model:				indicates the current CPLEX model
			float parameter:			indicates the upper bound in the objective function
			IloObjective objective:		indicates the current objective function
			IloRange information:		indicates the information constraint
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
	*@return void: -
	*********************************************************/
void model001(IloModel model, float parameter, IloObjective objective, IloRange information, IloNumVarArray W, IloNumVarArray Z, IloRangeArray T){
	IloConversion(env, W, IloNumVar::Float);
	information.setUb(parameter);
	information.setExpr(IloSum(Z));
	objective.setExpr(IloSum(W));
	objective.setSense(IloObjective::Maximize);
	model.remove(T);
}

/*
	Generates the constraints and objective function for model 002.

	In this model, Z is a binary variable and W is a binary variable.
	It aims to maximize the amount of reached members of the network (sum of W).
	It is constrained by the number of people that receives the initial information -> only maxInf nodes.
	Portuguese: Z binário, W binário, maximiza número de pessoas alcançadas (somatório de W) atendendo no máximo maxInf pessoas

	*@param IloModel model:				indicates the current CPLEX model
			float parameter:			indicates the upper bound in the objective function
			IloObjective objective:		indicates the current objective function
			IloRange information:		indicates the information constraint
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
	*@return void: -
*********************************************************/
void model002(IloModel model, float parameter, IloObjective objective, IloRange information, IloNumVarArray W, IloNumVarArray Z, IloRangeArray T) {
	IloConversion(env, W, IloNumVar::Bool);
	information.setUb(parameter);
	information.setExpr(IloSum(Z));
	objective.setExpr(IloSum(W));
	objective.setSense(IloObjective::Maximize);
	model.remove(T);
}

/*
	Generates the constraints and objective function for model 003.

	In this model, Z is a binary variable and W is a binary variable.
	It aims to minimize the amount of chosen members of the network to receive the initial information (sum of Z).
	It is constrained by attending at least a percentage of the network indicated by parameter.
	Portuguese: Z binário, W binário, minimiza número de pessoas escolhidas inicialmente (somatório de Z) atendendo no mínimo uma porcentagem da rede

	*@param IloModel model:				indicates the current CPLEX model
			float parameter:			indicates the percentage of people in the network that must be reached
			IloObjective objective:		indicates the current objective function
			IloRange information:		indicates the information constraint
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
	*@return void: -
*********************************************************/
void model003(IloModel model, float parameter, IloObjective objective, IloRange information, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z, IloRangeArray T){
	IloConversion(env, W, IloNumVar::Bool);
	objective.setExpr(IloSum(Z));
	objective.setSense(IloObjective::Minimize);
	information.setExpr(IloSum(W));
	information.setLb(allowedNodes->getSize()*parameter);
	model.remove(T);
}

/*
	Generates the constraints and objective function for model 004.

	In this model, a previous solution is passed by as parameter and the value of the objective function is calculated.
	Portuguese: Recebe solução inicial e verifica qual é o valor da funcao objetivo.

	*@param IloModel model:				indicates the current CPLEX model
			float parameter:			indicates the upper bound in the objective function
			IloObjective objective:		indicates the current objective function
			IloRange information:		indicates the information constraint
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloNumArray solution:		previous solution received as parameter
			IloRangeArray T:			constraint array that sets values to variables
	*@return void: -
*********************************************************/
void model004(IloModel model, float parameter, IloObjective objective, IloRange information, IloNumVarArray W, IloNumVarArray Z, IloNumArray solution, IloRangeArray T){
	IloConversion(env, W, IloNumVar::Float);
	objective.setExpr(IloSum(W));
	objective.setSense(IloObjective::Maximize);
	information.setExpr(-IloSum(Z));
	information.setLb(-IloInfinity);
	makeSolutionConstraint(T, Z, solution);
	model.add(T);
}

/*
	Generates the constraints and objective function for model 004.

	In this model, a previous solution is passed by as parameter and the value of the objective function is calculated.
	Portuguese: Recebe solução inicial e verifica qual é o valor da funcao objetivo.

	*@param IloModel model:				indicates the current CPLEX model
			float parameter:			indicates the upper bound in the objective function
			IloObjective objective:		indicates the current objective function
			IloRange information:		indicates the information constraint
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloNumArray solution:		previous solution received as parameter
			IloRangeArray T:			constraint array that sets values to variables
	*@return void: -
*********************************************************/
void model005(IloModel model, float parameter, IloObjective objective, IloRange information, IloNumVarArray W, IloNumVarArray Z, IloNumArray solution, IloRangeArray T){
	IloConversion(env, W, IloNumVar::Bool);
	objective.setExpr(IloSum(W));
	objective.setSense(IloObjective::Maximize);
	information.setUb(parameter);
	information.setExpr(IloSum(Z));
	makeSolutionConstraint(T, Z, solution);
	model.add(T);
}

string criteriaToString(int criteria){
	switch (criteria){
	case 0: return "SIW"; break;
	case 1: return "Degree"; break;
	case 2: return "Relative degree"; break;
	case 3: return "Closeness"; break;
	case 4: return "Relative closeness"; break;
	case 5: return "Eccentricity"; break;
	case 6: return "Radial"; break;
	}
	return "Not found";
}

/*
	Solves the previously constructed model
	*@param int modelNumber:			indicates the model that is being solved
			IloModel model:				indicates the current CPLEX model
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloRangeArray R:			is an array of constraints following the model
			IloNumArray solution:		stores a previous solution
			Graph *graph:				is a pointer to the graph
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			float parameter:			stores the information to set the upper bound of the information constraint
			float infCut:				determines when the information diffusion tree will be cut
			int initialC:				indicates the number of initial columns
			int timelimit:				indicates in minutes the time limit
			string append:				stores the path to the file to be written
			bool *generatedColumns:		generatedColumns[u] indicates whether a column for node u was generated or not
			int criteria:				indicates the criteria of sorting
	*@return void: -
*********************************************************/
int solve(int modelNumber, IloModel model, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R, IloNumArray solution, Graph *graph, Dictionary *allowedNodes, float parameter, float infCut, int initialC, int timelimit, string append, bool *generatedColumns, int criteria){
	
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t0);
	int nReached;
	ofstream output(append.c_str(), ios::app);

	stringstream auxText;
	IloRange information(env, 0, IloInfinity, "inf");
	IloObjective objective(env);
	IloRangeArray T(env);

	model.add(information);
	model.add(objective);

	switch (modelNumber){
		case 1: model001(model, parameter, objective, information, W, Z, T); break;
		case 2: model002(model, parameter, objective, information, W, Z, T); break;
		case 3: model003(model, parameter, objective, information, allowedNodes, W, Z, T); break;
		case 4: model004(model, parameter, objective, information, W, Z, solution, T); break;
		case 5: model005(model, parameter, objective, information, W, Z, solution, T); break;
		default: cout << "Wrong model parameter" << endl;
	}

	previousModel = modelNumber;

	IloCplex cplex(env);
	cplex.extract(model);

	cout << " > Setting CPLEX parameters ... ";
	parameters(cplex, initialC, allowedNodes->getSize(), timelimit, modelNumber);
	cout << "finished setting parameters!" << endl << endl;

	if (solution.getSize() != 0){
		try{
			cplex.addMIPStart(Z, solution, IloCplex::MIPStartAuto, "previousSolution");
		}
		catch (IloException e){
			cerr << e.getMessage() << endl;
		}
	}
	

	if (!cplex.solve()) {
		env.error() << " > Failed to optimize LP. Please go over the log file to find out what happened." << endl;
	}
	else{
		cplex.exportModel("lastModel.lp");			

		vector<int> solutionIDs, reachedIDs;
		int sTreeSize = 0, minTreeSize = HI, maxTreeSize = LI;
		float sInversedWeight = 0, minInversedWeight = HI, maxInversedWeight = LI;
		solution.clear();

		IloNum solutionValue = cplex.getObjValue();

		nReached = 0;
		env.out() << endl << "**************************************************************************" << endl;
		env.out() << " | MODEL 00" << modelNumber << endl;
		switch (modelNumber){
			case 1: env.out() << " | maxInf: " << parameter; break;
			case 2: env.out() << " | maxInf: " << parameter; break;
			case 3: env.out() << " | Percentage: " << parameter * 100 << "%"; break;
			case 4: env.out() << " | maxInf: " << parameter; break;
			case 5: env.out() << " | maxInf: " << parameter; break;
		}
		env.out() << "\tSolution status = " << cplex.getStatus() << "\tSolution value = " << solutionValue << endl;
		env.out() << " | infCut = " << infCut << "\tOrdering by: " << criteriaToString(criteria) << endl;
		env.out() << " > People who received original information:" << endl;
		for (int i = 0; i < allowedNodes->getSize(); i++){
			int node = allowedNodes->getNodeByIndex(i);
			if (generatedColumns[i] && cplex.getValue(Z[i]) == 1){
				solutionIDs.push_back(node);
				solution.add(1);
				Tree tree;
				graph->breadthSearch(&tree, node, infCut);
				env.out() << " | ID:  " << node << "\tSIW: " << graph->getSIW(node) << "\tTree size:  " << tree.getSize() << "\tIndex: " << i << endl;
				sInversedWeight += graph->getSIW(node);
				sTreeSize += tree.getSize();
				if (tree.getSize() < minTreeSize)
					minTreeSize = tree.getSize();
				if (tree.getSize() > maxTreeSize)
					maxTreeSize = tree.getSize();
				if (graph->getSIW(node) < minInversedWeight)
					minInversedWeight = graph->getSIW(node);
				if (graph->getSIW(node) > maxInversedWeight)
					maxInversedWeight = graph->getSIW(node);
			}
			else{
				solution.add(0);
			}
			if (cplex.getValue(W[i]) >= infCut){
				reachedIDs.push_back(node);
				nReached += graph->getReachedNodes(allowedNodes->getNodeByIndex(i));
			}
		}
		switch (modelNumber){
			case 1: printf(" | Reached without merged nodes %d\t\t\t\tPercentage: %.2f%%\n", nReached, ((float)nReached / graph->getNumberOfNodes()) * 100); break;
			case 2: printf(" | Reached without merged nodes %d\t\t\t\tPercentage: %.2f%%\n", nReached, ((float)nReached / graph->getNumberOfNodes()) * 100); break;
			case 3: printf(" | Number of selected people: %.0f\t\t\t\tPercentage: %.2f%%\n", solutionValue, ((float)solutionValue / graph->getNumberOfNodes()) * 100); break;
			case 4: printf(" | Reached without merged nodes %d\t\t\t\tPercentage: %.2f%%\n", nReached, ((float)nReached / graph->getNumberOfNodes()) * 100); break;
			case 5: printf(" | Reached without merged nodes %d\t\t\t\tPercentage: %.2f%%\n", nReached, ((float)nReached / graph->getNumberOfNodes()) * 100); break;
			default: cout << "Wrong model parameter" << endl;
		}
		printf(" | Reached counting merged nodes: %d\t\t\t\tPercentage: %.2f%%\n", nReached, ((float) nReached / graph->getNumberOfNodes()) * 100);
		env.out() << "**************************************************************************" << endl;

		QueryPerformanceCounter(&t1);

		//output << "model, parameter, nColumns, infCut, time, solutionValue, meanInversedWeight, minInversedWeight, maxInversedWeight, meanTreeSize, minTreeSize, maxTreeSize, pReached, nReached, solutionIDs, columnIDs, orderingBy, reachedIDs, ignored, comparison " << endl;
		
		output << modelNumber << "," << parameter << "," << initialC << "," << infCut << "," << time(&t1, &t0, &freq) << "," << solutionValue << ",";
		output << (solutionIDs.size() != 0 ? (float)(sInversedWeight / solutionIDs.size()) : -1) << "," << minInversedWeight << "," << maxInversedWeight << ",";
		output << (solutionIDs.size() != 0 ? (float)(sTreeSize / solutionIDs.size()) : -1) << "," << minTreeSize << "," << maxTreeSize << ",";
		output << ((float)nReached / allowedNodes->getSize()) * 100 << "," << nReached << ",";
		output << solutionIDs << "," << columnsIDs << "," << criteriaToString(criteria) << "," << reachedIDs << ",";
		output << ssIgnored.str() << "," << (comparison ? "high" : "low") << endl;

	}
	cplex.end();
	information.end();
	objective.end();
	return nReached;
}

/*
	Call the build and solve functions previously implemented
	*@param int modelNumber:			indicates the model that is being solved
			IloModel model:				indicates the current CPLEX model
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloRangeArray R:			is an array of constraints following the model
			IloNumArray solution:		stores a previous solution
			Graph *graph:				is a pointer to the graph
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			bool *generatedColumns:		generatedColumns[u] indicates whether a column for node u was generated or not
			float parameter:			stores the information to set the upper bound of the information constraint
			float infCut:				determines when the information diffusion tree will be cut
			int initialC:				indicates the number of initial columns
			int timelimit:				indicates in minutes the time limit
			string path:				stores the path to the file to be written
			int criteria:				indicates the criteria of sorting
	*@return void: -
*********************************************************/
int run(int modelNumber, IloModel model, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R, IloNumArray solution, Graph *graph, Dictionary *allowedNodes, bool * generatedColumns, float parameter, float infCut, int initialC, int timelimit, string path, int criteria){
	cout << endl << " $ Running model " << modelNumber << " with parameters: " << endl;
	cout << " | infCut: " << infCut << endl;
	cout << " | maxInf : " << parameter << endl;
	cout << " | nColumns : " << initialC << endl;
	cout << " | Ordering by: " << criteriaToString(criteria) << endl;
	ut.setBool(generatedColumns, allowedNodes->getSize(), false);
	columns(graph, generatedColumns, allowedNodes, R, Z, W, infCut, initialC, criteria, modelNumber);
	return solve(modelNumber, model, W, Z, R, solution, graph, allowedNodes, parameter, infCut, initialC, timelimit, path, generatedColumns, criteria);
}

/*
	Greedy approach to solve model 3
	*@param Graph *graph:				is a pointer to the graph
			float percentage:			indicates the percentage of people in the network that must be reached
	*@return void: -
*********************************************************/
void greedy(Graph * graph, float percentage){
	int counter = 0;
	int current = 0;
	int size = graph->getNumberOfNodes();
	heapNode * candidates;
	candidates = new heapNode[size];
	bool *chosen = new bool[size];
	for (int i = 0; i < size; i++){
		candidates[i].id = i + 1;
		candidates[i].chave = (float) graph->getDegree(i + 1);
		chosen[i] = false;
	}
	ut.heapSort(candidates, size);
	while (counter < percentage*graph->getNumberOfNodes()){
		current = 0;
		Tree tree;
		graph->breadthSearch(&tree, candidates[0].id, 0.001);
		cout << candidates[0].id << " | ";
		current++;
		candidates[0].chave = -1;
		for (int i = 0; i < tree.getSize(); i++){
			if (!chosen[tree.nodes[i] - 1]){
				chosen[tree.nodes[i] - 1] = true;
				current++;
			}
		}
		for (int i = 0; i < graph->getNumberOfNodes(); i++){
			if (chosen[candidates[i].id - 1])
				candidates[i].chave = -1;
		}
		ut.heapSort(candidates, size);
		size -= current;
		counter += current;
	}
}

/*
	Creates the ignored array based on specific criterias
	*@param Graph *graph:				is a pointer to the graph
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			int c1, c2:					indicate the criterias that will be considered in this run
			int nColumns:				indicates the number of columns that will be generated
	*@return void: -
*********************************************************/
void createIgnored(Graph *graph, Dictionary *allowedNodes, int c1, int c2, int nColumns){
	if (ignored != NULL) freeIgnored();
	ignored = new bool[graph->getNumberOfNodes()];
	ssIgnored.str("");
	ssIgnored << "WO considering columns in " << c1 << " and " << c2 << " at the same time";
	vector<int> listc1, listc2;
	graph->getInitialNodes(&listc1, allowedNodes, c1, nColumns);
	graph->getInitialNodes(&listc2, allowedNodes, c2, nColumns);
	bool *bc1 = new bool[graph->getNumberOfNodes()];
	bool *bc2 = new bool[graph->getNumberOfNodes()];
	for (int i = 0; i < graph->getNumberOfNodes(); i++){
		bc1[i] = false;
		bc2[i] = false;
	}
	for(int j = 0; j < listc1.size(); j++){
		bc1[listc1[j] - 1] = true;
		bc2[listc2[j] - 1] = true;
	}
	for (int i = 0; i < graph->getNumberOfNodes(); i++){
		ignored[i] = bc1[i] && bc2[i];
	}
}

int independentCascade(Graph *g, list<int> chosen, int newnode, float infCut){
	int sum = 0;
	vector<int> partial;
	Tree tree;
	bool *setunion = new bool[g->getNumberOfNodes()];
	ut.setBool(setunion, g->getNumberOfNodes(), false);
	chosen.push_back(newnode);
	cout << "#";
	while (!chosen.empty()){
		partial.push_back(chosen.front());
		chosen.pop_front();
	}
	for (int node : partial){
		g->breadthSearchW(&tree, node, infCut);
		for (int i = 0; i < tree.getSize(); i++){
			setunion[tree.nodes[i] - 1] = setunion[tree.nodes[i] - 1] && tree.info[i];
		}
	}

	for (int i = 0; i < g->getNumberOfNodes(); i++){
		sum += setunion[i];
	}
	
	return sum;
}

int linearThreshold(Graph *g, list<int> chosen, int newnode, float infCut){
	bool change = true;
	int sum = 0;
	vector<int> adj;
	float *sumWeights = new float[g->getNumberOfNodes()];
	float *previousSumWeights = new float[g->getNumberOfNodes()];
	vector<int> current;
	for (int c = 0; c < g->getNumberOfNodes(); c++){
		sumWeights[c] = 0;
		previousSumWeights[c] = 0;
	}
	cout << "Linear Threshold Model" << endl;
	while (!chosen.empty()){
		current.push_back(chosen.front());
		previousSumWeights[chosen.front() - 1] = 1;
		chosen.pop_front();
	}
	while (change){

		change = false;
		for (int i = 0; i < g->getNumberOfNodes(); i++)
			previousSumWeights[i] = sumWeights[i];

		for(int node : current){
			g->getAdjacency(&adj, node);
			for (int a : adj){
				sumWeights[a - 1] += g->getWeight(a, node)*(previousSumWeights[node - 1] > infCut? 1 : 0);
			}
		}

		sum += (int) current.size();
		current.clear();

		for (int i = 0; i < g->getNumberOfNodes(); i++){
			if (previousSumWeights[i] == 0 && sumWeights[i] != 0){
				current.push_back(i + 1);
				change = true;
			}
		}

	}
	return sum;
}

void greedyApproach(Graph *g, float infCut, int k, char type = 'c'){
	int count = 0;
	int sum = 0;
	int highestSum = 0;
	int highestSumNode = -1;
	list<int> set;
	while (count < k){
		cout << "Looking for node with highest sum... " << endl;
		for (int i = 0; i < g->getNumberOfNodes(); i++){
			switch (type) {
				case 't': sum = linearThreshold(g, set, i + 1, infCut); break;
				case 'c': sum = independentCascade(g, set, i + 1, infCut); break;
				default: exit(0);
			}
			
			if (sum > highestSum){
				highestSum = sum;
				highestSumNode = i + 1;
			}
		}
		cout << highestSumNode << " found" << endl;
		set.push_back(highestSumNode);
	}
	cout << "Set: ";
	while (!set.empty()){
		cout << set.front() << " | ";
		set.pop_front();
	}
}

void createSolution(Graph *graph, Dictionary *allowedNodes, IloNumArray solution, int criteria, int amount){
	vector<int> nodes;
	graph->getInitialNodes(&nodes, allowedNodes, criteria, amount);
	for (int i = 0; i < allowedNodes->getSize(); i++)
		solution.add(0);
	for (int node : nodes){
		solution[allowedNodes->getIndexByNode(node)] = 1;
	}
}

int main(int argc, char **argv){
	
	Graph graph;
	graph.load("lorenza.txt");
	Dictionary allowedNodes(graph.getNumberOfNodes());
	
	IloModel model(env);
	IloNumVarArray W(env);
	IloNumVarArray Z(env);
	IloRangeArray R(env);
	IloRangeArray T(env);
	IloNumArray solution(env);

	build(&graph, &allowedNodes, W, Z, R, T);

	model.add(R);
	bool *generatedColumns = new bool[allowedNodes.getSize()];

	int maxInf = 10;
	int tlimit = 60;
	int ncolumns = 1000;
	string file = "newts.csv";
	int value = 0;
	float infCuts[] = { (float) 0.001, (float) 0.005, (float) 0.01, (float) 0.1 };

	ofstream output("prettyOutput.txt", ios::app);
	output << "maxinf, infCut, approach, amountReached, percentageReached" << endl;

	for (int i = 0; i < 4; i++){
		for (int j = 15; j > 0; j--){

			// our approach
			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, j, infCuts[i], ncolumns, tlimit, file, 0);
			value = run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, j, infCuts[i], ncolumns, tlimit, file, 0);
			output << j << ", " << infCuts[i] << ", MIP, " << value << ", " << (float) value / graph.getNumberOfNodes() << endl;

			// selecting by centrality measures
			for (int k = 1; k < 7; k++){
				createSolution(&graph, &allowedNodes, solution, k, j);
				value = run(5, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, j, infCuts[i], ncolumns, 1, file, 0);
				output << j << ", " << infCuts[i] << ", " << criteriaToString(k) << ", " << value << ", " << (float) value / graph.getNumberOfNodes() << endl;
			}
		}
	}
}
