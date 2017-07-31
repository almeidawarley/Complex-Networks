#include "stdafx.h"

Data::Data(int i, int n){
	index = i;
	node = n;
	next = NULL;
}

Data::~Data(){
	delete next;
}

int Data::getIndex(){
	return index;
}

int Data::getNode(){
	return node;
}

Data* Data::getNext(){
	return next;
}

void Data::setNext(Data* d){
	next = d;
}

void Data::setIndex(int i){
	index = i; 
}

void Data::setNode(int n){
	node = n;
}