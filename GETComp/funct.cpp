/**
	Information Diffusion in Complex Networks
	funct.cpp
	Purpose: implements the class Funct

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

Funct::Funct(){

}

Funct::~Funct(){

}

void Funct::wait(string text = ""){
	cout << text << " -> Enter <- " << endl;
	getchar();
}

void Funct::setInt(int* vector, int size, int value){
	for (int u = 0; u < size; u++)
		vector[u] = value;
}

void Funct::setBool(bool* vector, int size, bool value){
	for (int u = 0; u < size; u++)
		vector[u] = value;
}

/*
	Function to show percentage of loading on the screen
	*@param int current: information about the current stage
			int total: information about the final stage
			int *control: pointer to variable that stores the last printed percentage
			int size: size of the array to be sorted
	*@return void: -
*********************************************************/
void Funct::load(int current, int total, double *control){
	if ((double) current / total > *control){
		*control = *control + 0.01;
		printf("\b\b\b\b%3.f%%", *control * 100);
	}
}

/*
	Function to perform heapify in an array
	*@param heapNode *v: array of heap nodes to be sorted
			int index: index from which the heapify will go on
			int size: size of the array to be sorted
	*@return void: -
*********************************************************/
void Funct::heapify(heapNode *heap, int index, int size){
	int biggest = index;
	int left = 2 * index + 1;
	int right = 2 * index + 2;
	if (left < size && heap[index].chave > heap[left].chave){
		biggest = left;
	}
	if (right < size && heap[biggest].chave > heap[right].chave){
		biggest = right;
	}
	if (biggest != index){
		heapNode aux = heap[index];
		heap[index] = heap[biggest];
		heap[biggest] = aux;
		heapify(heap, biggest, size);
	}
}

/*
	Function to perform heap sort in an array
	*@param heapNode *heap: array to be sorted
			int size: size of the array to be sorted
	*@return void: -
*********************************************************/
void Funct::heapSort(heapNode* heap, int size){
	for (int i = size / 2; i >= 0; i--){
		heapify(heap, i, size);
	}
	while (size > 0){
		heapNode aux = heap[0];
		heap[0] = heap[size - 1];
		heap[size - 1] = aux;
		heapify(heap, 0, size - 1);
		size--;
	}
}