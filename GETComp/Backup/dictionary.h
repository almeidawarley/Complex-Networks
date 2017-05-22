#include "stdafx.h"


using namespace std;

class Dictionary{
	
	public:
		//Dictionary();
		Dictionary(int t);
		~Dictionary();
		int add(int n);
		void print();
		//void remove(int n);
		int getIndexByNode(int n);
		int getNodeByIndex(int i);
		int getSize();
		void free();

	private:
		/*Data* first;
		Data* last;*/
		int lastIndex;

		int size;
		int *info;
};

/*
	Código para teste da estrutura

	int a = 0, b = 1;
	while (a != b){
	cout << "Digite o vertice: "; cin >> a;
	d.add(a);
	d.print();
	cout << endl << "Digite o vertice para busca: "; cin >> b;
	cout << "Index de " << b << ": " << d.getIndexByNode(b) << endl;


*/