#include "Tree.h"
#include "stdafx.h"

using namespace std;

class Graph{
    private:
		int nNodes;
		int nArcs;
		int nDisconnected;
		int nMerged;
        Arc **arcs;
        int *sizes;
		int *bFlags;	
		int *dFlags;
		float *sumInversedWeight;
		int *merge;
		int *amountReached;
		/*
			Brief description of some atributes:
			- int sizes[u] contains the number of adjacent nodes of u;
			- int *flags is an auxiliar array used during searches;
		*/

    public:
        Graph();
        ~Graph();
		void loadFromFile(string path, string apath);
        float getWeight(int o, int d);
        int getNumberOfNodes();
		int getNumberOfArcs();
		void getAdjacency(list<int> *l,int n);
		float getSumOfInversedWeights(int n);
		bool isConnected(int n);
		void breadthSearch(Tree *tree, int n, double cut);
		void depthSearch(list<int> *depth, int node, int maxSize);
		void getInitialVertexes(list<int> *r, bool criteria = true, int s = -1);
		void print();
		void setNumberOfDisconnected();
		int getNumberOfDisconnected();
		int getMerge(int n);
		int getDegree(int n);
		void orderBy(list<int> *set, int criteria, int size);
		void getPathSizeAtAllNodes(int n);

		void excent(int n);
		void brandes();
};