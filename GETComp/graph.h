/**
	Information Diffusion in Complex Networks
	graph.h
	Purpose: defines the class Graph

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

/**
	Brief description of the metrics:

	# SIW (sum of inversed weights)		highlights nodes that are important for their neighbours
	# DEGREE							highlights nodes that have many conections
	# RDEGREE (relative degree)			highlights nodes that have a good proportion between their conections and the number of nodes
	# CLOSENESS							highlights nodes that have less distance to all the others nodes
	# RCLOSENESS (relative closeness)	highlights nodes that have a good proportion between their distance to all the other nodes and the number of nodes
	# ECCENTRICITY						highlights nodes that have small maximum distance from all the nodes  
	# RADIAL							highlights nodes that are more integrated into the network

*/

#define SIW 0
#define DEGREE 1
#define RDEGREE 2
#define CLOSENESS 3
#define RCLOSENESS 4
#define ECCENTRICITY 5
#define RADIAL 6

using namespace std;

class Graph{

	/**
		Brief description of the atributes:
		- int nNodes			stores the number of nodes in the graph
		- int nArcs				stores the number of arcs in the graph
		- int nDisconnected		stores the number of disconnected nodes in the graph
		- int nMerged			stores the number of merged nodes in the graph
		- Arc **arcs			arcs[u] is an array which stores the arcs departing from node u
		- int *sizes			sizes[u] stores the degree of node u
		- int *bFlags			bFlags is a flag array used in breadth search
		- int *dFlags			dFlags is a flag array used in depth search
		- float *siw			siw[u] stores the sum of inversed weights of node u
		- int *merge			merge[u] stores how many nodes were merged into node u
	*/

    private:
		int nNodes;
		int nArcs;
		int nDisconnected;
		int nMerged;
        Arc **arcs;
        int *sizes;
		int *bFlags;	
		int *dFlags;
		int *merge;
		float *siw;

    public:

        Graph();
        ~Graph();
		void loadFromFile(string path);

        int getNumberOfNodes();
		int getNumberOfArcs();
		int getNumberOfDisconnected();
		int getMerge(int n);
		int getDegree(int n);
		void setNumberOfDisconnected();
		float getSIW(int n);

		void getInitialNodes(list<int> *r, Dictionary *allowedNodes, int criteria, int amount = -1);
		void getAdjacency(list<int> *l, int n);
		float getWeight(int o, int d);
		
		bool isConnected(int n);
		void breadthSearch(Tree *tree, int n, double cut);
		void depthSearch(list<int> *depth, int node, int maxSize);		
		void print();

		void getPathSizeAtAllNodes(int n);
		void eccentricity(int n);
};