
#include "stdafx.h"

/*Dictionary::Dictionary(){ 
	first = NULL;
	last = NULL;
	lastIndex = 0;
}*/

Dictionary::Dictionary(int t){
	size = t;
	info = new int[t];
	lastIndex = 0;
	for (int u = 0; u < size; u++)
		info[u] = -1;
}

Dictionary::~Dictionary(){

}

int Dictionary::add(int n){
	/*Data* d = new Data(lastIndex,n);
	lastIndex++;
	if (first == NULL&&last == NULL){
		first = d;
		last = d;
	}
	else{
		last->setNext(d);
		last = d;
	}*/
	info[lastIndex] = n;
	lastIndex++;
	return lastIndex - 1;
}

int Dictionary::getIndexByNode(int n){
	/*Data* aux;
	aux = first;
	while (aux != NULL){
		if (aux->getNode() == n){
			return aux->getIndex();
		}
		aux = aux->getNext();
	}
	return -1;*/
	int i;
	for (i = 0; i < size; i++){
		if (info[i] == n){
			return i;
		}
	}
	return -1;
}

int Dictionary::getNodeByIndex(int i){
	/*Data* aux;
	aux = first;
	while (aux != NULL){
		if (aux->getIndex() == i){
			return aux->getNode();
		}
		aux = aux->getNext();
	}
	return -1;*/
	return info[i];
}

int Dictionary::getSize(){
	return lastIndex;
}

void Dictionary::print(){
	/*Data* aux; 
	aux = first;
	cout << "D: ";
	while (aux != NULL){
		cout << "(" << aux->getIndex() << ", " << aux->getNode() << "), ";
		aux = aux->getNext();
	}*/
}

void Dictionary::free(){
	delete info;
	info = NULL;
	size = -1;
	lastIndex = -1;
	cout << "Excluindo..." << endl;
}