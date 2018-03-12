/**
	Information Diffusion in Complex Networks
	tree.cpp
	Purpose: implements the class Tree

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

Tree::Tree(){
	lastAdded = 0;
	nodes = NULL;
	info = NULL;
	index = NULL;	
	level = NULL;
}

void Tree::build(int s, int n){
	nodes = new int[s];
	info = new float[s];
	index = new int[s];
	level = new int[s];
	size = s;
	for (int u = 0; u < s; u++){
		info[u] = 0;
		index[u] = -1;
		nodes[u] = -1;
		level[u] = 0;
	}
	root = n;
}

Tree::~Tree(){
	//cout << "Destruindo tree" << endl;
	delete[] info;
	delete[] index;
	delete[] nodes;
	delete[] level;
}

void Tree::add(int v, int p, float i){
	index[v - 1] = lastAdded;
	nodes[lastAdded] = v;
	info[lastAdded] = i;

	if (index[p - 1] != -1)
		level[lastAdded] = level[index[p - 1]] + 1;
	else
		level[lastAdded] = 1;
	lastAdded++;
}

void Tree::print(){
	int count = 0;
	stringstream path;
	path << "from" << root << ".txt";
	ofstream output(path.str().c_str());
	output << "from " << root << endl;
	for (int u = 0; u < lastAdded; u++){
		//int n = index[u];
		//if (index[u] != -1){
			output << "[ Vertice: " << nodes[u] << " | Info: " << info[u] << " | Nivel: " << level[u] << "]" << endl;
			//cout << "[ Vertice: " << u+1 << " | Info: " << info[index[u]] << " | Nivel: " << level[index[u]] << "]" << endl;
			//count++;
		//}
	}
	//cout << "Elementos validos: " << count << endl;
}

float Tree::getInfoByVertex(int v){
	int n = index[v - 1];
	if (n == -1)
		return 0;
	return info[n];
}

float Tree::getInfoParent(int p){
	int n = index[p - 1];
	if (n == -1)
		return 1;
	return info[n];
}

int Tree::getSize(){
	return lastAdded;
}

int Tree::getRoot(){
	return root;
}

void Tree::reachedList(list<int> *r){
	for (int u = 0; u < size; u++){
		if (index[u] != -1)
			r->push_back(u + 1);
	}
}

float Tree::getInfo(int index){
	if (index >= 0 && index < lastAdded)
		return info[index];
	else
		return -1;
}

int Tree::getNode(int index){
	if (index >= 0 && index < lastAdded)
		return nodes[index];
	else
		return -1;
}
