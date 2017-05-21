#include <iostream>
#include "stdafx.h"

using namespace std;

MaxHeap::MaxHeap(int mSize){
    maxSize = mSize;
    data = new Data*[maxSize];
    lastIndex = -1;
    for(int u = 0; u < maxSize; u++){
        data[u] = NULL;
    }
}

MaxHeap::~MaxHeap()
{
    for(int u = 0; u < maxSize; u++){
        delete data[u];
    }
    data = NULL;
}

int MaxHeap::parent(int i){
    return (int) ((i-1)/2);
}

int MaxHeap::child(int i){
    if((2*i+1) <= lastIndex && (2*i+2) <= lastIndex){
        if(data[2*i+1]!=NULL && data[2*i+2]!=NULL){
            if(data[2*i+1]->getWeight() > data[2*i+2]->getWeight()){
                return (int) 2*i + 1;
            }else{
                return (int) 2*i + 2;
            }
        }
        if(data[2*i+1]==NULL && data[2*i+2]==NULL){
            return (int) 2*i+1;
        }
        if(data[2*i+1]==NULL){
            return (int) 2*i+2;
        }
        if(data[2*i+2]==NULL){
            return (int) 2*i+1;
        }
    }
	return lastIndex+1;
}

bool MaxHeap::add(int i, float ar){
    lastIndex++;
    Data *d = new Data();
    d->setData(i, ar);
    data[lastIndex] = d;
	siftUp();
    return true;

}

void MaxHeap::print(){
    for(int u = 0; u < lastIndex+1; u++)
        cout << "[" << data[u]->getWeight() << "]\t";
    cout << endl;
}

int MaxHeap::extract(){
    int r = data[0]->getId();
    Data * aux = data[0];
    delete aux;
	aux = NULL;
    data[0] = data[lastIndex];
    data[lastIndex] = NULL;
    lastIndex -- ;
	siftDown();
    return r;
}

bool MaxHeap::empty(){
	if (lastIndex == -1){
		return true;
	}
	else{
		return false;
	}
}

void MaxHeap::siftDown(){
	//cout << "Antes: " << endl;
	//print();
	int searchIndex = 0;
	int currentIndex = 0;
	//cout << "child(searchIndex): " << child(searchIndex) << endl;
	//cout << "lastIndex: " << lastIndex << endl;
	while (child(searchIndex) <= lastIndex){
		searchIndex = child(searchIndex);
		//cout << "searchIndex: " << data[searchIndex]->getWeight() << endl;
		//cout << "currentIndex: " << " : " << data[currentIndex]->getWeight() << endl;
		if (data[searchIndex]->getWeight() > data[currentIndex]->getWeight()){
			//cout << "Trocando [" << data[searchIndex]->getWeight() << "] com [" << data[currentIndex]->getWeight() << "] "<< endl;
			Data *aux = data[searchIndex];
			data[searchIndex] = data[currentIndex];
			data[currentIndex] = aux;
			currentIndex = searchIndex;
			aux = NULL;
		}
		//cout << "-><-" << endl;
	}
	//cout << "Depois: " << endl;
	//print();
}

void MaxHeap::siftUp(){
	int currentIndex = lastIndex;
	int searchIndex = lastIndex;
	while (searchIndex != 0){
		searchIndex = parent(searchIndex);
		if (data[searchIndex]->getWeight() < data[currentIndex]->getWeight()){
			//cout << "Trocando [" << data[searchIndex]->getWeight() << "] com [" << data[currentIndex]->getWeight() << "] "<< endl;
			Data *aux = data[searchIndex];
			data[searchIndex] = data[currentIndex];
			data[currentIndex] = aux;
			currentIndex = searchIndex;
			aux = NULL;
		}
	}
}

void MaxHeap::sort(list<int> *set, int stop){
	Funct utilities;
	if (stop == -1)
		stop = lastIndex + 1;
	//cout << "Limite: " << stop << endl;
	double control = 0;
	cout << "> Sorting candidates:      ";
	int counter = 0;
	while (counter < stop){
		int aux = extract();
		//cout << "Adicionando " << aux << endl;
		set->push_back(aux);
		counter++;
		utilities.load((int)set->size(), stop, &control);
	}
}

int MaxHeap::size(){
	return lastIndex + 1;
}