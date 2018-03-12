/**
	Information Diffusion in Complex Networks
	tree.h
	Purpose: defines the class Tree

	@author Warley Almeida Silva
	@version 1.0
*/

#ifndef TREE_H
#define TREE_H
#include "stdafx.h"

class Tree{
	private:
		int *index;
		int *level;
		int lastAdded;
		int size;
		int root;

	public:
		int *nodes;
		float *info;
        Tree();
        ~Tree();
		void build(int s, int n);
        void add(int v, int p, float i);
		float getInfoByVertex(int v);
		float getInfoParent(int p);
		void reachedList(list<int> *r);
		void print();
		int getSize();
		int getRoot();
		float getInfo(int index);
		int getNode(int index);
};

#endif // TREE_H
