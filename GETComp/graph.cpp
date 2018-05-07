/**
	Information Diffusion in Complex Networks
	graph.cpp
	Purpose: implements the class Graph

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

Graph::Graph(){
	nNodes = 0;
	nArcs = 0;
	nDisconnected = 0;
	nMerged = 0;
    arcs = NULL;
	sizes = NULL;
	siw = NULL;
}

Graph::~Graph(){
    delete[] arcs;
    delete[] sizes;
	delete[] dFlags;
	delete[] bFlags;
	delete[] siw;
}

/*
	Function to load the graph from a file
	*@param  string path: constains the name of the file
	*@return void: -
*********************************************************/
void Graph::load(string path){

	/*
		Declaration
	*/

	ifstream input(path.c_str());
	int origin, endpoint, counter = 0, merged = 0, size;
	float weight;
	input >> nNodes;
	siw = new float[nNodes];
	sizes = new int[nNodes];
	arcs = new Arc*[nNodes];
	dFlags = new int[nNodes];
	bFlags = new int[nNodes];
	merge = new int[nNodes];
	double border = 0;	

	/*
		Initialization
	*/

	for (int u = 0; u < nNodes; u++){
		arcs[u] = NULL;
		sizes[u] = -1;
		siw[u] = 0;
		dFlags[u] = 0;
		bFlags[u] = 0;
		merge[u] = 0;
	}
	cout << "> Loading graph:     ";

	/*
		Reading file
	*/

	while (input >> origin >> size){
		if ((float) origin / nNodes > border){
			printf("\b\b\b\b%3.f%%", border * 100);
			border += 0.01;
		}
		if (size == 1){
			input >> endpoint >> weight;
			merge[endpoint - 1]++;
			merged++;
		}
		else{
			arcs[origin - 1] = new Arc[size];
			sizes[origin - 1] = size;
			while (counter < size){
				input >> endpoint >> weight;
				arcs[origin - 1][counter].setEndpoint(endpoint);
				arcs[origin - 1][counter].setWeight(weight);
				siw[endpoint - 1] += weight;
				nArcs++;
				counter++;
			}
			counter = 0;
		}

	}
	nMerged = merged;
	printf("\b\b\b\b100%\n");
	cout << "| Number of nodes: " << nNodes << endl;
	cout << "| Number of arcs: " << nArcs << endl;
	cout << "| Number of merged nodes: " << nMerged << endl;
	setNumberOfDisconnected();
	cout << "| Number of disconnected nodes: " << nDisconnected << endl;
}

/*
	Function to get the weight between two nodes
	*@param int o: constains the origin of the arc
			int d: constains the endpoint of the arc
	*@return float: weight of the arc
*********************************************************/
float Graph::getWeight(int o, int d){
	if ((arcs[o - 1] != NULL || sizes[o - 1] != -1) && o <= nNodes && d <= nNodes && o > 0 && d > 0){
		for (int i = 0; i < sizes[o - 1]; i++){
			if (arcs[o - 1][i].getEndpoint() == d){
				return arcs[o - 1][i].getWeight();
			}
		}
	}
	return 0;
}

/*
	Function to get the adjacency of a node
	*@param int n: constains the node
			list<int> *l: list containing the adjacency of n
	*@return void: -
*********************************************************/
void Graph::getAdjacency(vector<int> *l, int n){
	if (n <= nNodes && n > 0){
		for (int i = 0; i < sizes[n - 1]; i++){
			if (isConnected(arcs[n - 1][i].getEndpoint()))
				l->push_back(arcs[n - 1][i].getEndpoint());
		}
	}
}

/*
	Function to build information spreading tree from node n
	*@param int n: constains the node
			double cut: defines when to stop
			Tree *tree is the tree we're building
	*@return void: -
*********************************************************/
void Graph::breadthSearch(Tree *tree, int n, double cut){
	tree->build(nNodes, n);
	Queue queue(nNodes);
	Funct utilities;
	queue.in(n);
	utilities.setInt(bFlags, nNodes, -1);
	while (!queue.empty()){
		int t = queue.next();
		queue.out();
		if (bFlags[t - 1] != n){
			float weight = tree->getInfoParent(t);
			if (weight >= cut){
				for (int c = 0; c < sizes[t - 1]; c++){
					int currentV = arcs[t - 1][c].getEndpoint();
					if (bFlags[currentV - 1] != n && isConnected(currentV) && !queue.contains(currentV)){
						queue.in(currentV);
						tree->add(currentV, t, weight*getWeight(currentV, t));
					}
				}
				bFlags[t - 1] = n;
			}
		}
	}
}

void Graph::breadthSearchW(Tree *tree, int n, double cut){
	tree->build(nNodes, n);
	Queue queue(nNodes);
	Funct utilities;
	queue.in(n);
	utilities.setInt(bFlags, nNodes, -1);
	while (!queue.empty()){
		int t = queue.next();
		queue.out();
		if (bFlags[t - 1] != n){
			float weight = tree->getInfoParent(t);
			for (int c = 0; c < sizes[t - 1]; c++){
				int currentV = arcs[t - 1][c].getEndpoint();
				if (bFlags[currentV - 1] != n && weight*getWeight(currentV, t) > cut && isConnected(currentV) && !queue.contains(currentV)){
					queue.in(currentV);
					tree->add(currentV, t, 1);
				}
			}
			bFlags[t - 1] = n;
		}
	}
}

/*
	Function to generate list of better nodes according to some criteria
	*@param list<int> *r: pointer to the list that will receive the initial nodes
			Dictionary *allowedNodes: dictionary with all allowed nodes
			int criteria: defines the criteria to be used while sorting
			int s: defines the amount of nodes that can be selected
	*@return void: -
*********************************************************/
void Graph::getInitialNodes(vector<int> *r, Dictionary *allowedNodes, int criteria, int amount){
	heapNode *ord = new heapNode[nNodes];
	Funct utilities;
	ifstream input;
	switch (criteria){
	case SIW: break;
	case DEGREE: break;
	case RDEGREE:  input.open("criterio2.txt"); break;
	case CLOSENESS: input.open("criterio3.txt"); break;
	case RCLOSENESS: input.open("criterio4.txt"); break;
	case ECCENTRICITY: input.open("criterio5.txt"); break;
	case RADIAL: input.open("criterio6.txt"); break;
	default: cout << "An error was found while opening file" << endl;
	}
	int temp;
	if (input.is_open()) // reads first line
		input >> temp;
	for (int i = 0; i < nNodes; i++){
		switch (criteria){
		case SIW:			ord[i].id = i + 1; ord[i].chave = siw[i]; break;
		case DEGREE:		ord[i].id = i + 1; ord[i].chave = (float) sizes[i]; break;
		case RDEGREE:		input >> temp; ord[temp - 1].id = temp; input >> ord[temp - 1].chave; break;
		case CLOSENESS:		input >> temp; ord[temp - 1].id = temp; input >> ord[temp - 1].chave; break;
		case RCLOSENESS:	input >> temp; ord[temp - 1].id = temp; input >> ord[temp - 1].chave; break;
		case ECCENTRICITY:	input >> temp; ord[temp - 1].id = temp; input >> ord[temp - 1].chave; break;
		case RADIAL:		input >> temp; ord[temp - 1].id = temp; input >> ord[temp - 1].chave; break;
		default: cout << "An error was found during initial vertexes generation" << endl;
		}
	}
	utilities.heapSort(ord, nNodes);
	if (amount > nNodes){
		amount = nNodes - 1;
	}
	for (int c = 0; r->size() != amount && c < amount; c++){
		if (allowedNodes->getIndexByNode(ord[c].id) != -1){
			r->push_back(ord[c].id);
		}
	}
	delete[] ord;
}

/*
	Function to do a depth search starting at some specific node
	*@param list<int> *depth: pointer to the list that will receive nodes found in the depth search
			int node: node from which the search will begin
			int maxSize: defines the maximum number of nodes that can be discovered
	*@return void: -
*********************************************************/

void Graph::depthSearch(list<int> *depth, int node, int maxSize){
	list<int> stack;
	vector<int> adj;
	stack.push_front(node);
	Funct utilities;
	//cout << "Depth from " << n << ": ";
	while (!stack.empty()){
		int t = stack.front();
		stack.pop_front();
		if (dFlags[t - 1] != node){
			getAdjacency(&adj, t);
			for (int a : adj){
				if (a != node)
					stack.push_back(a);
			}
			depth->push_back(t);
			dFlags[t - 1] = node;
		}
		if (depth->size() > maxSize)
			break;
	}
}

/*
	Function to sum the path size from a node n to all nodes in the graph
	*@param int node: node from which the sum will be calculated
	*@return void: -
*********************************************************/
void Graph::getPathSizeAtAllNodes(int n){
	ofstream output("redoPathAtAllNodes.txt", ios::app);
	int *distances = new int[nNodes];
	int *queue = new int[nNodes];
	int i, sum, bQueue, eQueue, node, indexDistance, listSize;
	register int j, front, index;
	for (i = 0; i < nNodes; i++){
		distances[i] = 0;
		queue[i] = -1;
	}
	for (i = n - 1; i < nNodes; i++){
		sum = 0;
		bQueue = 0;
		eQueue = 0;
		node = i + 1;
		queue[eQueue++] = node;
		distances[i] = 0;
		while (eQueue > bQueue){
			index = queue[bQueue++] - 1;
			indexDistance = distances[index] + 1;
			listSize = sizes[index];
			for (j = 0; j < listSize; j++){
				front = arcs[index][j].endpoint - 1;
				if (front + 1 != node && distances[front] == 0 &&
					!(arcs[front] == NULL)){ // if this node isn't in the list already
					queue[eQueue++] = front + 1;
					distances[front] = indexDistance;
					sum += indexDistance;
				}
			}
		}
		output << i + 1 << " " << sum << endl;
		for (j = 0; j < eQueue; j++){
			distances[queue[j] - 1] = 0;
		}
	}
}

/*
	Function to calculate the eccentricity of node n
	*@param int n: node from which the eccentricity will be calculated
	*@return void: -
*********************************************************/

void Graph::eccentricity(int n){
	ofstream output("redoEccentricity.txt", ios::app);
	int *distances = new int[nNodes];
	int *queue = new int[nNodes];
	int bQueue, eQueue, node, indexDistance, listSize;
	register int i, j, front, index;
	for (i = 0; i < nNodes; i++){
		distances[i] = 0;
		queue[i] = -1;
	}
	for (i = n - 1; i < nNodes; i++){
		bQueue = 0;
		eQueue = 0;
		node = i + 1;
		queue[eQueue++] = node;
		distances[i] = 0;
		while (eQueue > bQueue){
			index = queue[bQueue++] - 1;
			indexDistance = distances[index] + 1;
			listSize = sizes[index];
			for (j = 0; j < listSize; j++){
				front = arcs[index][j].endpoint - 1;
				if (front + 1 != node && distances[front] == 0 && !(arcs[front] == NULL)){
					queue[eQueue++] = front + 1;
					distances[front] = indexDistance;
				}
			}
		}
		int max = 0;
		for (j = 0; j < eQueue; j++){
			if (distances[j] > max)
				max = distances[j];
			distances[queue[j] - 1] = 0;
		}
		output << i + 1 << " " << max << endl;
	}
}

/*
	Function to decide either a node n is connected (useful when handling merged nodes)
	*@param  int n: constains the node
	*@return bool: true if is connected, false when it isn't
*********************************************************/
bool Graph::isConnected(int n){
	return !(arcs[n - 1] == NULL || sizes[n - 1] == -1);
}

void Graph::setNumberOfDisconnected(){
	for (int u = 0; u < getNumberOfNodes(); u++){
		if (arcs[u] == NULL){
			nDisconnected++;
		}
	}
}

int Graph::getNumberOfDisconnected(){
	return nDisconnected;
}

int Graph::getNumberOfNodes(){
    return nNodes;
}

int Graph::getNumberOfArcs(){
	return nArcs;
}

float Graph::getSIW(int n){
	return siw[n - 1];
}

int Graph::getReachedNodes(int n){
	return merge[n - 1] + 1;
}

int Graph::getDegree(int n){
	return sizes[n - 1];
}

void Graph::print(){
	cout << "Graph: " << endl;
	for (int c = 0; c < nNodes; c++){
		cout << "{id: " << c + 1 << ", m: " << merge[c] <<  "} -> ";
		for (int d = 0; d < sizes[c]; d++){
			cout << "[" << arcs[c][d].getEndpoint() << "], ";
		}
		cout << endl;
	}
}