#include "stdafx.h"
#define INF 99999999

Graph::Graph(){
    arcs = NULL;
    sizes = NULL;
    nNodes = 0;
    nArcs = 0;
	sumInversedWeight = NULL;
	amountReached = NULL;
	nDisconnected = 0;
	nMerged = 0;
}

Graph::~Graph(){
    delete[] arcs;
    delete[] sizes;
	delete[] dFlags;
	delete[] bFlags;
	delete[] sumInversedWeight;
	delete[] amountReached;
}

/*
	Function to load the graph from a file
	*@param  string path: constains the name of the file
	*@return void: -
*********************************************************/
void Graph::loadFromFile(string path, string apath){
	/*
		Declaration
	*/
	ifstream input(path.c_str());
	//ofstream logs("[l] graphLoading.txt");
	int origin, endpoint, counter = 0, merged = 0, size;
	float weight;
	input >> nNodes;
	sumInversedWeight = new float[nNodes];
	amountReached = new int[nNodes];
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
		sumInversedWeight[u] = 0;
		dFlags[u] = 0;
		bFlags[u] = 0;
		amountReached[u] = 0;
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
		//logs << "Lendo origem " << origin << " e tamanho " << size << "..." << endl;
		if (size == 1){
			input >> endpoint >> weight;
			merge[endpoint - 1]++;
			//logs << "No " << origin << " ignorado por ter peso igual a um" << endl;
			//logs << "Arco com origem " << origin << " e destino " << endpoint << " ignorado por ter sido fundido" << endl;
			merged++;
		}
		else{
			arcs[origin - 1] = new Arc[size];
			sizes[origin - 1] = size;
			while (counter < size){
				input >> endpoint >> weight;
				arcs[origin - 1][counter].setEndpoint(endpoint);
				arcs[origin - 1][counter].setWeight(weight);
				sumInversedWeight[endpoint - 1] += weight;
				nArcs++;
				//logs << "Arco com destino " << endpoint << " e peso " << weight << " adicionado" << endl;
				counter++;
			}
			counter = 0;
		}
		//logs << "arcs[" << origin - 1 << "] : " << arcs[origin - 1] << " sizes[" << origin - 1 << "] : " << sizes[origin - 1] << endl;
		//logs << endl;

	}
	nMerged = merged;
	printf("\b\b\b\b100%\n");
	cout << "| Number of nodes: " << nNodes << endl;
	cout << "| Number of arcs: " << nArcs << endl;
	cout << "| Number of merged nodes: " << nMerged << endl;
	setNumberOfDisconnected();
	cout << "| Number of disconnected nodes: " << nDisconnected << endl;
	//ofstream output("[i] Criterio1.txt");
	//output << nNodes << endl;
	//for (int u = 0; u < nNodes; u++)
	//	output << u + 1 << " " << sizes[u] << "\n";
	//input.close();

	/*input.open(apath.c_str());
	while (input >> origin >> size){
		amountReached[origin - 1] = size;
	}
	input.close();*/
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

/*
	Function to get the weight between two nodes
	*@param  int o: constains the origin of the arc
			 int d: constains the endpoint of the arc
	*@return float: weight of the arc
*********************************************************/
float Graph::getWeight(int o, int d){
    if((arcs[o-1] != NULL || sizes[o-1]!=-1) && o<= nNodes && d <= nNodes && o > 0 && d > 0){
        for(int i = 0; i < sizes[o-1]; i++){
            //cout << "Acessando no de origem " << o << ", " << i << " componente , " << sizes[o-1] << " de tamanho" << endl;
            if(arcs[o-1][i].getEndpoint() == d){
                return arcs[o-1][i].getWeight();
            }
        }
    }
    return 0;
}

int Graph::getNumberOfNodes(){
    return nNodes;
}

int Graph::getNumberOfArcs(){
	return nArcs;
}

/*
	Function to get the adjacency of a node
	*@param  int n: constains the node
			 list<int> *l: list containing the adjacency of n
	*@return void: -
*********************************************************/
void Graph::getAdjacency(list<int> *l, int n){
	if (n <= nNodes && n > 0){
		for (int i = 0; i < sizes[n-1]; i++){
			if(isConnected(arcs[n - 1][i].getEndpoint()))
				l->push_back(arcs[n-1][i].getEndpoint());
		}
	}
}

float Graph::getSumOfInversedWeights(int n){
	return sumInversedWeight[n - 1];
}

/*
	Function to decide either a node n is connected (useful when handling merged nodes)
	*@param  int n: constains the node
	*@return bool: true if is connected, false when it isn't
*********************************************************/
bool Graph::isConnected(int n){
	return !(arcs[n - 1] == NULL || sizes[n - 1] == -1);
}

/*
	Function to build information spreading tree from node n
	*@param  int n: constains the node
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
	//cout << "Breadth from " << n << ": ";
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

/*struct est{
	float chave;
	int id;
} typedef node;

void heapfy(node *v, int ind, int tam){
	int maior = ind;
	int esq = 2 * ind + 1;
	int dir = 2 * ind + 2;
	if (esq < tam && v[ind].chave > v[esq].chave){
		maior = esq;
	}
	if (dir < tam && v[maior].chave > v[dir].chave){
		maior = dir;
	}
	if (maior != ind){
		node aux = v[ind];
		v[ind] = v[maior];
		v[maior] = aux;
		heapfy(v, maior, tam);
	}
}

void heapSort(node* v, int tam){
	for (int i = tam / 2; i >= 0; i--){
		heapfy(v, i, tam);
	}
	while (tam > 0){
		node aux = v[0];
		v[0] = v[tam - 1];
		v[tam - 1] = aux;
		heapfy(v, 0, tam - 1);
		tam--;
	}

}*/

struct est{
	float chave;
	int id;
} typedef node;

void heapfy(node *v, int ind, int tam){
	int maior = ind;
	int esq = 2 * ind + 1;
	int dir = 2 * ind + 2;
	if (esq < tam && v[ind].chave > v[esq].chave){
		maior = esq;
	}
	if (dir < tam && v[maior].chave > v[dir].chave){
		maior = dir;
	}
	if (maior != ind){
		node aux = v[ind];
		v[ind] = v[maior];
		v[maior] = aux;
		heapfy(v, maior, tam);
	}
}

void heapSort(node* v, int tam){
	for (int i = tam / 2; i >= 0; i--){
		heapfy(v, i, tam);
	}
	while (tam > 0){
		node aux = v[0];
		v[0] = v[tam - 1];
		v[tam - 1] = aux;
		heapfy(v, 0, tam - 1);
		tam--;
	}

}

void Graph::getInitialVertexes(list<int> *r, bool criteria, int s){
	node *ord = new node[nNodes];
	for (int i = 0; i < nNodes; i++){
		ord[i].id = i + 1;
		switch (criteria){
			case 1: ord[i].chave = sumInversedWeight[i]; break;
			case 2: ord[i].chave = sizes[i]; break;
			default: ord[i].chave = sumInversedWeight[i]; break;
		}
	}
	heapSort(ord, nNodes);
	for (int c = 0; c < s; c++){
		r->push_front(ord[c].id);
	}
	delete[] ord;
}

void Graph::depthSearch(list<int> *depth, int node, int maxSize){
	list<int> stack;
	list<int> adj;
	stack.push_front(node);
	Funct utilities;
	//cout << "Depth from " << n << ": ";
	while (!stack.empty()){
		int t = stack.front();
		stack.pop_front();
		if (dFlags[t - 1] != node){
			getAdjacency(&adj, t);
			while (!adj.empty()){
				if (adj.front() != node)
					stack.push_back(adj.front());
				adj.pop_front();
			}
			depth->push_back(t);
			dFlags[t - 1] = node;
		}
		if (depth->size() > maxSize)
			break;
	}
}

void Graph::print(){
	cout << "Graph: " << endl;
	for (int c = 0; c < nNodes; c++){
		cout << "{" << c + 1 << "} -> ";
		for (int d = 0; d < sizes[c]; d++){
			cout << "[" << arcs[c][d].getEndpoint() << "], ";
		}
		cout << endl;
	}
	/*cout << "SumInversedWeight: " << endl;
	for (int c = 0; c < nNodes; c++){
		cout << c + 1 << " -> " << sumInversedWeight[c] << endl;
	}*/
}

float min(float a, float b){
	if (a < b)
		return a;
	return b;
}

int Graph::getMerge(int n){
	return merge[n - 1] + 1;
}

int Graph::getDegree(int n){
	return sizes[n - 1];
}

void Graph::orderBy(list<int> *set, int criteria, int size){
	MaxHeap aux(getNumberOfNodes());
	for (int c = 0; c < getNumberOfNodes(); c++){
		switch (criteria){
			case 1: aux.add(c + 1, getDegree(c + 1)); break;
			case 2: aux.add(c + 1, (getDegree(c + 1) / (float) getNumberOfNodes())); break;
			default: aux.add(c+1, getSumOfInversedWeights(c + 1)); break;
		}
	}
	aux.sort(set, size);
	cout << endl;
}

void Graph::getPathSizeAtAllNodes(int n){
	ofstream output("pathAtAllNodes.txt", ios::app);
	int *distances = new int[nNodes];
	int *queue = new int[nNodes];
	int i, sum, bQueue, eQueue, node, indexDistance,listSize;
	register int j,front,index;
	for (i = 0; i < nNodes; i++){
		distances[i] = 0;
		queue[i] = -1;
	}
	for (i = n-1; i < nNodes; i++){
		sum = 0;
		bQueue = 0;
		eQueue = 0;
		node = i + 1;
		queue[eQueue++] = node;
		distances[i] = 0;
		//cout << "NODE: " << node << endl;
		while (eQueue > bQueue){
			//cout << endl << endl << "BQUEUE: " << bQueue << " EQUEUE: " << eQueue << endl;
			//getchar();
			//cout << "\tanalysing " << queue[bQueue] << endl;
			//while (!adj.empty()){
			index = queue[bQueue++] - 1;
			indexDistance = distances[index] + 1;
			listSize = sizes[index];
			for (j = 0; j < listSize; j++){
				front = arcs[index][j].endpoint- 1;
				if (front + 1 != node && distances[front] == 0 &&
					!(arcs[front] == NULL )){ // if this node isn't in the list already
					//cout << "\t\tstocking " << front << endl;
					queue[eQueue++] = front + 1;
					distances[front] = indexDistance;
					sum += indexDistance;
				}
			}
		}
		//for (int c = 0; c < nNodes; c++)
			//cout << c + 1 << " | " << distances[c] << " ; ";
		//cout << "sum " << sum << endl; 

		output << i + 1 << " " << sum << endl;
		//cout << "a";
		for (j = 0; j < eQueue; j++){
			//if (queue[j] == -1)
			//	cout << "error" << endl;
			distances[queue[j] - 1] = 0;
		}
		//cout << "b";
	}
}

void Graph::excent(int n){
	ofstream output("excent.txt", ios::app);
	//output << nNodes << endl;
	int *distances = new int[nNodes];
	int *queue = new int[nNodes];
	int bQueue, eQueue, node, indexDistance, listSize;
	register int i, j, front, index;
	for (i = 0; i < nNodes; i++){
		distances[i] = 0;
		queue[i] = -1;
	}
	for (i = n-1; i < nNodes; i++){
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

void Graph::brandes(){
	//int limit = (nNodes*nNodes - 3 * nNodes + 2);
	Funct ut;
	float *cb = new float[nNodes];
	list<int> stack;
	list<int> *lists = new list<int>[nNodes];
	list<int> queue;

	int *distance = new int[nNodes];
	int *sigma = new int[nNodes];
	float *delta = new float[nNodes];

	for (int c = 0; c < nNodes; c++)
		cb[c] = 0;
	cout << "Brandes:       ";
	double control = 0;
	register int s, t, j;
	for (s = 0; s < nNodes; s++){
		ut.load(s, nNodes, &control);
		queue.push_back(s);
		for (t = 0; t < nNodes; t++){
			distance[t] = -1;
			sigma[t] = 0;
			delta[t] = 0;
		}
		distance[s] = 0;
		sigma[s] = 1;

		while (!queue.empty()){
			int v = queue.front();
			queue.pop_front();
			stack.push_front(v);
			for (j = 0; j < sizes[v]; j++){
				int w = arcs[v][j].getEndpoint() - 1;
				if (distance[w] < 0){
					queue.push_back(w);
					distance[w] = distance[v] + 1;
				}
				if (distance[w] == distance[v] + 1){
					sigma[w] = sigma[w] + sigma[v];
					lists[w].push_back(v);
				}
			}			
		}
		while (!stack.empty()){
			int w = stack.front();
			stack.pop_front();
			while (!lists[w].empty()){
				int v = lists[w].back();
				lists[w].pop_back();
				delta[v] = delta[v] + (1 + delta[w])*((float)sigma[v] / (float)sigma[w]);
			}
			if (w != s){
				cb[w] = cb[w] + delta[w];
			}
		}
	}
	ofstream output("cB.txt");
	output << nNodes << endl;
	for (int l = 0; l < nNodes; l++)
		output << l + 1 << " " << (cb[l]/2) << endl;

	delete[]delta;
	delete[]sigma;
	delete[]distance;
	delete[]cb;
	delete[]lists;
}