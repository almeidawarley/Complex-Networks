/**
	Information Diffusion in Complex Networks
	dictionary.h
	Purpose: defines the class Dictionary

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

using namespace std;

class Dictionary{
	
	public:
		Dictionary(int t);
		~Dictionary();
		int add(int n);
		void print();
		int getSize();
		int getIndexByNode(int n);
		int getNodeByIndex(int i);

	private:

		int lastIndex;
		int size;
		int *info;
		int *indexes;
};