/**
	Information Diffusion in Complex Networks
	funct.h
	Purpose: defines the class Funct

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

struct nodeStr{
	float chave;
	int id;
} typedef heapNode;

class Funct{
	private:
		void heapify(heapNode *v, int ind, int tam);	

	public:
		Funct();
		~Funct();
		void wait(string text);
		void setInt(int* vector, int size, int value);
		void setBool(bool* vector, int size, bool value);
		void load(int current, int total, double *control);
		void heapSort(heapNode* v, int tam);	
		void error(string text);
};

