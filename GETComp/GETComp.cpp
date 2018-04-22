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
#define LBMAXTIME 60
#define LBMINTIME 10
#define LBTIMEUNIT 10
#define LBMAXRADIUS 5
#define LBMINRADIUS 1
#define LBRADIUSUNIT 1

using namespace std;

list<int> testeG;

Funct utilities;
LARGE_INTEGER t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, freq;
bool *ignored = NULL;
bool comparison = true;
stringstream ssColumns, ssSolution, ssIgnored;

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
	delete[]ignored;
	ignored = NULL;
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
		utilities.wait("Erro ao gerar colunas");
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
			utilities.load(f, tree.getSize(), &control);
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
	*@param IloEnv env:					indicates the current CPLEX library environment
			Graph *graph:				is a pointer to the graph
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			IloRangeArray R:			is an array of constraints following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloNumVarArray W:			is an array with variables W following the model
	*@return void: -
*********************************************************/
void build(IloEnv env, Graph *graph, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R){
	double control = 0;
	stringstream auxText;
	Funct utilities;
	int index;
	cout << "> Building:      ";
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
			utilities.load(u, graph->getNumberOfNodes(), &control);
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
	list<int> allowedZ;
	IloEnv env = R.getEnv();
	graph->getInitialNodes(&allowedZ, allowedNodes, orderingBy, amount);
	cout << "> Columns:      ";
	double control = 0;
	int initialSize = (int) allowedZ.size();
	while (!allowedZ.empty()){
		createColumn(graph, generatedColumns, allowedZ.front(), allowedNodes, R, Z, W, cut, model, false);
		ssColumns << allowedZ.front() << " ";
		allowedZ.pop_front();
		utilities.load(initialSize - (int) allowedZ.size(), initialSize, &control);
	}
	cout << endl << endl;
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
void parameters(IloCplex cplex, int initialC, int initialR, int timelimit, int model, bool branching){
	cplex.setParam(IloCplex::TiLim, timelimit * 60);
	cplex.setParam(IloCplex::MemoryEmphasis, true);
	cplex.setParam(IloCplex::WorkMem, 100);
	cplex.setParam(IloCplex::NodeFileInd, 1);
	cplex.setParam(IloCplex::ColReadLim, initialC);
	cplex.setParam(IloCplex::RowReadLim, initialR);
	cplex.setParam(IloCplex::NumericalEmphasis, 1);
	cplex.setParam(IloCplex::PolishAfterDetTime, 2 * 30 * 60);
	cplex.setParam(IloCplex::RINSHeur, 30);

	if (branching){
		cplex.setParam(IloCplex::LBHeur, 1);
		cout << "LB heuristic: " << cplex.getParam(IloCplex::LBHeur) << endl;
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
			IloRange infFunction:		indicates the information constraint
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
	*@return void: -
	*********************************************************/
void model001(IloModel model, float parameter, IloObjective objective, IloRange infFunction, IloNumVarArray W, IloNumVarArray Z){
	try{
		IloEnv env = model.getEnv();
		infFunction.setUb(parameter);
		objective.setExpr(IloSum(W));
		objective.setSense(IloObjective::Maximize);
		infFunction.setExpr(IloSum(Z));
		IloConversion(env, W, IloNumVar::Float);
	}
	catch (IloException e){
		cerr << e << endl;
	}
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
			IloRange infFunction:		indicates the information constraint
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
	*@return void: -
*********************************************************/
void model002(IloModel model, float parameter, IloObjective objective, IloRange infFunction, IloNumVarArray W, IloNumVarArray Z) {
	try{	
		IloEnv env = model.getEnv();
		infFunction.setUb(parameter);
		objective.setExpr(IloSum(W));
		objective.setSense(IloObjective::Maximize);
		infFunction.setExpr(IloSum(Z));
		IloConversion(env, W, IloNumVar::Bool);
	}
	catch (IloException e){
		cerr << e << endl;
	}
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
			IloRange infFunction:		indicates the information constraint
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
	*@return void: -
*********************************************************/
void model003(IloModel model, float parameter, IloObjective objective, IloRange infFunction, Dictionary *allowedNodes, IloNumVarArray W, IloNumVarArray Z){
	IloEnv env = model.getEnv();
	try{		
		objective.setExpr(IloSum(Z));
		objective.setSense(IloObjective::Minimize);
		infFunction.setExpr(IloSum(W));
		infFunction.setLb(allowedNodes->getSize()*parameter);
		IloConversion(env, W, IloNumVar::Bool);
	}
	catch (IloException e){
		cerr << e << endl;
	}
}

/*
	Generates the constraints and objective function for model 004.

	In this model, a previous solution is passed by as parameter and the value of the objective function is calculated.
	Portuguese: Recebe solução inicial e verifica qual é o valor da funcao objetivo.

	*@param IloModel model:				indicates the current CPLEX model
			float parameter:			indicates the upper bound in the objective function
			IloObjective objective:		indicates the current objective function
			IloRange infFunction:		indicates the information constraint
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloNumArray solution:		previous solution received as parameter
			IloRangeArray T:			constraint array that sets values to variables
	*@return void: -
*********************************************************/
void model004(IloModel model, float parameter, IloObjective objective, IloRange infFunction, IloNumVarArray W, IloNumVarArray Z, IloNumArray solution, IloRangeArray T){
	try{
		IloEnv env = model.getEnv();
		objective.setExpr(IloSum(W));
		objective.setSense(IloObjective::Maximize);
		infFunction.setExpr(-IloSum(Z));
		infFunction.setLb(-IloInfinity);
		IloConversion(env, W, IloNumVar::Float);
		for (int i = 0; i < solution.getSize(); i++)
			if (solution[i] > 0.9)
				T.add(Z[i] == 1);

	}
	catch (IloException e){
		cerr << e << endl;

	}
}

/*
	Generates the constraints and objective function for model 004.

	In this model, a previous solution is passed by as parameter and the value of the objective function is calculated.
	Portuguese: Recebe solução inicial e verifica qual é o valor da funcao objetivo.

	*@param IloModel model:				indicates the current CPLEX model
			float parameter:			indicates the upper bound in the objective function
			IloObjective objective:		indicates the current objective function
			IloRange infFunction:		indicates the information constraint
			IloNumVarArray W:			is an array with variables W following the model
			IloNUmVarArray Z:			is an array with variables Z following the model
			IloNumArray solution:		previous solution received as parameter
			IloRangeArray T:			constraint array that sets values to variables
	*@return void: -
*********************************************************/
void model005(IloModel model, float parameter, IloObjective objective, IloRange infFunction, IloNumVarArray W, IloNumVarArray Z, IloNumArray solution, IloRangeArray T){
	try{
		IloEnv env = model.getEnv();
		objective.setExpr(IloSum(W));
		objective.setSense(IloObjective::Maximize);
		infFunction.setUb(parameter);
		infFunction.setExpr(IloSum(Z));
		infFunction.setLb(-IloInfinity);
		IloConversion(env, W, IloNumVar::Bool);
		for (int i = 0; i < solution.getSize(); i++)
			if (solution[i] == 1)
				T.add(Z[i] == 1);

	}
	catch (IloException e){
		cerr << e << endl;

	}
}

/*
	Sets a local branching constraint based on a previous solution
	*@param IloRange localB:			stores the local branching constraint
			IloNumArray solution:		stores a previous solution
			IloNUmVarArray Z:			is an array with variables Z following the model
			Dictionary *allowedNodes:	dictionary with all allowed nodes
			int radius:					can be used to choose even less members from previous solution
	*@return void: -
*********************************************************/
void localBranching(IloRange localB, IloNumArray solution, IloNumVarArray Z,  Dictionary *allowedNodes, int radius){
	cout << "Adding local branching constraint" << endl;
	int counter = 0;
	for (int u = 0; u < solution.getSize(); u++){
		counter += int(solution[u]);
	}
	cout << "Counter: " << counter << endl;
	localB.setLinearCoefs(Z, solution);
	localB.setLB(counter - radius);

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
			bool branching:				indicates whether a local branching constraint will be added
	*@return void: -
*********************************************************/
void solve(int modelNumber, IloModel model, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R, IloNumArray solution, Graph *graph, Dictionary *allowedNodes, float parameter, float infCut, int initialC, int timelimit, string append, bool *generatedColumns, int criteria, bool branching = false, int radius = 0){
	
	ofstream dataOutput(append.c_str(), ios::app);
	QueryPerformanceCounter(&t0);
	QueryPerformanceFrequency(&freq);

	stringstream auxText;
	IloEnv env = model.getEnv();
	IloCplex cplex(model);
	IloRange infFunction(env, 0, IloInfinity, "inf");
	IloRange localB(env, -IloInfinity, IloInfinity, "localB");
	IloObjective objective(env);
	IloRangeArray T(env);

	model.add(infFunction);
	model.add(localB);
	model.add(objective);	
	
	QueryPerformanceCounter(&t5);
	
	switch (modelNumber){
		case 1: model001(model, parameter, objective, infFunction, W, Z); break;
		case 2: model002(model, parameter, objective, infFunction, W, Z); break;
		case 3: model003(model, parameter, objective, infFunction, allowedNodes, W, Z); break;
		case 4: model004(model, parameter, objective, infFunction, W, Z, solution, T); break;
		case 5: model005(model, parameter, objective, infFunction, W, Z, solution, T); break;
		default: cout << "Wrong model parameter" << endl; exit(1);
	}
	QueryPerformanceCounter(&t6);
	bool repeat;
	int previousSolution = MAXINT;
	int iteration = 0;

	/*if (branching){
		timelimit = (LBMAXTIME + LBMINTIME) / 2;
		radius = (LBMAXRADIUS + LBMINRADIUS) / 2;
	}*/

	

	do{
		bool hasPreviousSolution = false;
		repeat = false;
		iteration++;

		QueryPerformanceCounter(&t7);
		parameters(cplex, initialC, allowedNodes->getSize(), timelimit, modelNumber, branching);

		if (solution.getSize() != 0){
			/*if (branching){
				localBranching(localB, solution, Z, allowedNodes, radius);
				cout << "Local branching constraint added to the model" << endl;
			}*/
			try{
				cplex.addMIPStart(Z, solution, IloCplex::MIPStartAuto, "previousSolution");
			}
			catch (IloException e){
				cout << "Previous solution could not be used by solver" << endl;
				cout << Z.getSize() << " - " << solution.getSize() << endl;
				hasPreviousSolution = false;
				cerr << e << endl;
			}
		}

		if (!cplex.solve()) {
			env.error() << "Failed to optimize LP." << endl;
		}
		else{
			try{
				QueryPerformanceCounter(&t8);

				//auxText << "model00" << modelNumber << ".lp";
				//cplex.exportModel(auxText.str().c_str());
				//auxText.str("");

				/*if (branching){
					if (previousSolution > cplex.getObjValue()){
						repeat = true;
						if (timelimit > LBMINTIME)
							timelimit -= LBTIMEUNIT;
						if (radius < LBMAXRADIUS)
							radius += LBRADIUSUNIT;
					}
					else{
						if (timelimit < LBMAXTIME || radius > LBMINRADIUS){
							repeat = true;
							if (timelimit < LBMAXTIME)
								timelimit += LBTIMEUNIT;
							if (radius > LBMINRADIUS)
								radius -= LBRADIUSUNIT;
						}
						else{
							cout << "# Local Branching could not improve solution and will be therefore terminated" << endl;
						}
					}
				}
				previousSolution = cplex.getObjValue();*/
				

				cout << "Repeat: " << repeat << " | Branching: " << branching << " | Timelimit: " << timelimit << " | Radius: " << radius << " | iteration: " << iteration << endl;

				QueryPerformanceCounter(&t9);
				stringstream ids;
				IloNumArray valuesW(env);
				IloNumArray valuesZ(env);
				cplex.getValues(Z, valuesZ);
				cplex.getValues(W, valuesW);
				int sTreeSize = 0, minTreeSize = HI, maxTreeSize = LI;
				float sInversedWeight = 0, minInversedWeight = HI, maxInversedWeight = LI;
				solution.clear();

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
				env.out() << "\tSolution status = " << cplex.getStatus() << "\tSolution value = " << solutionValue << "\tinfCut  = " << infCut << "\tCriteria: " << criteriaToString(criteria) << endl;
				env.out() << " > People who received original information:" << endl;
				for (int i = 0; i < allowedNodes->getSize(); i++){
					int node = allowedNodes->getNodeByIndex(i);
					if (generatedColumns[i] && cplex.getValue(Z[i]) > 0.9){
						ids << node << " ";
						solution.add(1);
						inSolution++;
						Tree tree;
						graph->breadthSearch(&tree, node, infCut);
						env.out() << " | ID:  " << node << "\tSIW: " << graph->getSIW(node) << "\tTree size:  " << tree.getSize() << "\tI: " << i << endl;
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
						reached++;
						ssSolution << node << " ";
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
				case 5: printf(" | Reached without merge %d\t\t\t\tPercentage: %.2f%%\n", reached, ((float)reached / allowedNodes->getSize()) * 100); break;
				default: cout << "Wrong model parameter" << endl;
				}
				printf(" | Reached with merge: %d\t\t\t\tPercentage: %.2f%%\n", counter, ((float)counter / allowedNodes->getSize()) * 100);
				env.out() << "**************************************************************************" << endl;
				
				QueryPerformanceCounter(&t10);

				dataOutput << modelNumber << ";" << parameter << ";" << initialC << ";" << infCut << ";" << time(&t10, &t0, &freq) << ";";
				dataOutput << time(&t2, &t1, &freq) << ";" << time(&t4, &t3, &freq) << ";" << time(&t6, &t5, &freq) << ";" << time(&t8, &t7, &freq) << ";" << time(&t10, &t9, &freq) << ";";
				dataOutput << solutionValue << ";" << (inSolution != 0 ? (float)(sInversedWeight / inSolution) : -1) << ";" << minInversedWeight << ";" << maxInversedWeight << ";";
				dataOutput << (inSolution != 0 ? (float)(sTreeSize / inSolution) : -1) << ";" << minTreeSize << ";" << maxTreeSize << ";" << ((float)counter / allowedNodes->getSize()) * 100 << ";" << counter << ";";
				dataOutput << branching << ";" << iteration << ";" << hasPreviousSolution << ";" << ids.str() << ";" << ssColumns.str() << ";" << criteria << ";" << ssSolution.str() << ";" << ssIgnored.str() << ";" << (comparison ? "high" : "low") << ";" << radius << ";" << endl;
				ssColumns.str("");
				ssSolution.str("");
			}
			catch (IloException e){
				cerr << e << endl;
				cout << e.getMessage() << endl;
			}
			catch (exception& e){
				cerr << e.what() << endl;
			}
		}
	} while (repeat && branching && iteration < 10);
	cplex.end();
	infFunction.end();
	objective.end();
	localB.end();
	T.end();
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
			bool branching:				indicates whether a local branching constraint will be added
	*@return void: -
*********************************************************/
void run(int modelNumber, IloModel model, IloNumVarArray W, IloNumVarArray Z, IloRangeArray R, IloNumArray solution, Graph *graph, Dictionary *allowedNodes, bool * generatedColumns, float parameter, float infCut, int initialC, int timelimit, string path, int criteria, bool branching  = false, int radius = 0){
	cout << "Calling for " << infCut << " - " << parameter << " - " << initialC << endl;
	utilities.setBool(generatedColumns, allowedNodes->getSize(), false);
	columns(graph, generatedColumns, allowedNodes, R, Z, W, infCut, initialC, criteria, modelNumber);
	solve(modelNumber, model, W, Z, R, solution, graph, allowedNodes, parameter, infCut, initialC, timelimit, path, generatedColumns, criteria, branching, radius);
	for (int i = 0; i < R.getSize(); i++)
		R[i].setExpr(W[i]);
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
		candidates[i].chave = graph->getDegree(i + 1);
		chosen[i] = false;
	}
	utilities.heapSort(candidates, size);
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
		utilities.heapSort(candidates, size);
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
	list<int> listc1, listc2;
	graph->getInitialNodes(&listc1, allowedNodes, c1, nColumns);
	graph->getInitialNodes(&listc2, allowedNodes, c2, nColumns);
	bool *bc1 = new bool[graph->getNumberOfNodes()];
	bool *bc2 = new bool[graph->getNumberOfNodes()];
	for (int i = 0; i < graph->getNumberOfNodes(); i++){
		bc1[i] = false;
		bc2[i] = false;
	}
	while (!listc1.empty()){
		bc1[listc1.front() - 1] = true;
		bc2[listc2.front() - 1] = true;
		listc1.pop_front();
		listc2.pop_front();
	}
	for (int i = 0; i < graph->getNumberOfNodes(); i++){
		ignored[i] = bc1[i] && bc2[i];
	}
}

int independentCascade(Graph *g, list<int> chosen, int newnode, float infCut){
	int front;
	int sum = 0;
	vector<int> partial;
	Tree tree;
	bool *setunion = new bool[g->getNumberOfNodes()];
	utilities.setBool(setunion, g->getNumberOfNodes(), false);
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
	list<int> adj;
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
			int front;
			g->getAdjacency(&adj, node);
			while (!adj.empty()){
				front = adj.front();
				adj.pop_front();
				sumWeights[front - 1] += g->getWeight(front, node)*(previousSumWeights[node - 1] > infCut? 1 : 0);
			}
		}

		sum += current.size();
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
	list<int> answer;
	graph->getInitialNodes(&answer, allowedNodes, criteria, amount);
	for (int i = 0; i < solution.getSize(); i++)
		solution[i] = 0;
	while (!answer.empty()){
		solution[allowedNodes->getIndexByNode(answer.front())] = 1;
		answer.pop_front();
	}
}

int main(int argc, char **argv){	

	QueryPerformanceCounter(&t0);

	// (1) Declaring variables

	Graph graph;
	graph.loadFromFile("lorenza.txt");
	Dictionary allowedNodes(graph.getNumberOfNodes());
	

	//greedyApproach(&graph, 0.001, 5);
	//utilities.wait("fim do algoritmo guloso");

	IloEnv env;
	IloModel model(env);
	IloNumVarArray W(env);
	IloNumVarArray Z(env);
	IloRangeArray R(env);
	IloRangeArray T(env);

	QueryPerformanceCounter(&t1);
	build(env, &graph, &allowedNodes, W, Z, R);
	QueryPerformanceCounter(&t2);

	IloNumArray solution(env);

	model.add(R);

	bool *generatedColumns = new bool[allowedNodes.getSize()];
	float percentages[] = {0.1, 0.2, 0.3, 0.4, 0.5};
	int maxInfs[] = { 5, 15, 25, 35, 45 };
	float infCuts[] = { 0.001, 0.005, 0.01, 0.1 };
	int maxInf = 10;

	// (2) Running the tests

	int tlimit = 30;
	int ncolumns = 1000;
	string file = "newtests.csv";

	for (int i = 0; i < 4; i++){
		for (int j = 1; j < 30; j++){

			// our approach
			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, j, infCuts[i], ncolumns, tlimit, file, 0);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, j, infCuts[i], ncolumns, tlimit, file, 0);

			// selecting by centrality measures
			for (int k = 1; k < 7; k++){
				createSolution(&graph, &allowedNodes, solution, k, j);
				run(5, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, j, infCuts[i], ncolumns, 1, file, 0);
			}
		}
	}

	

	utilities.wait("opa");

	// Independent Cascade Model
	/*run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, 1);

	list<int> adj;
	for (int i = 0; i < solution.getSize(); i++)
		if (solution[i])
			adj.push_back(allowedNodes.getNodeByIndex(i));
	int ops = independentCascade(&graph, adj, 2, 0.01);
	cout << (float)ops/graph.getNumberOfNodes() << endl;*/


	//Tests for ignoring common and uncommon columns
	/*for (int c = 0; c < 7; c++){
		for (int d = c+1; d < 7 && c!= d; d++){
			createIgnored(&graph, &allowedNodes, c, d, ncolumns);

			comparison = false;

			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, c);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, c, true);

			comparison = true;

			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, c);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, c, true);

			comparison = false;

			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, d);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, d, true);

			comparison = true;

			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, d);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, 15, 0.001, ncolumns, tlimit, file, d, true, 3);

			solution.clear();
			freeIgnored();
		}
	}*/


	//Basic test for models 1 and 2
	/*for (int c = 0; c < 5; c++){
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 5; j++){
				run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], ncolumns, tlimit, file, c);
				run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], ncolumns, tlimit, file, c);
				run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], ncolumns, tlimit, file, c);
				solution.clear();
			}
		}
	}*/
	utilities.wait("FIM");

	//Tests for model 1 and 3
	/*for (int i = 0; i < PARAMETERSIZE - 1; i++){
		for (int j = 0; j < PARAMETERSIZE; j++){
			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInf, infCuts[i], 1000, 180, "criterio3.csv");
			run(3, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, percentages[j], infCuts[i], 1000, 180, "criterio3.csv");
			run(4, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInf, infCuts[i], 1000, 180, "criterio3.csv");
			solution.clear();
		}
	}*/

	
	/*for (int i = 0; i < PARAMETERSIZE - 1; i++){
		for (int j = 0; j < PARAMETERSIZE; j++){
			run(3, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, percentages[j], infCuts[i], 1000, 30, "criterio3.csv", SIW);
			run(4, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInf, infCuts[j], 1000, 30, "criterio3.csv", SIW);
			solution.clear();
		}
	}*/
	
	//Tests for model 1 and 2
	/*for (int i = 0; i < 4; i++){
		for (int j = 0; j < 5; j++){
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio2.csv", RDEGREE);
			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio2.csv", RDEGREE);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio2.csv", RDEGREE);
			solution.clear();
		}
	}
	for (int i = 1; i < 4; i++){
		for (int j = 0; j < 5; j++){
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio3.csv", CLOSENESS);
			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio3.csv", CLOSENESS);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio3.csv", CLOSENESS);
			solution.clear();
		}
	}
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 5; j++){
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio4.csv", RCLOSENESS);
			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio4.csv", RCLOSENESS);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio4.csv", RCLOSENESS);
			solution.clear();
		}
	}
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 5; j++){
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio5.csv", ECCENTRICITY);
			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio5.csv", ECCENTRICITY);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio5.csv", ECCENTRICITY);
			solution.clear();
		}
	}
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 5; j++){
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio6.csv", RADIAL);
			run(1, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio6.csv", RADIAL);
			run(2, model, W, Z, R, solution, &graph, &allowedNodes, generatedColumns, maxInfs[j], infCuts[i], 1000, 30, "criterio6.csv", RADIAL);
			solution.clear();
		}
	}
	*/
	
}
