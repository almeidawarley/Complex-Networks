#include "stdafx.h"

using namespace std;

class Data{
	private:
		int index, node;
		Data* next;
	public:
		Data(int i, int n);
		~Data();
		int getIndex();
		int getNode();
		void setIndex(int i);
		void setNode(int n);
		Data* getNext();
		void setNext(Data* d);
	
};