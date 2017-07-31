#include "stdafx.h"

Tupla::Tupla(int i, int o, int d){
	index = i;
	origin = o;
	endpoint = d;
	next = NULL;
}

Tupla::~Tupla(){
	delete next;
}

int Tupla::getIndex(){
	return index;
}

int Tupla::getEndpoint(){
	return endpoint;
}

int Tupla::getOrigin(){
	return origin;
}

Tupla* Tupla::getNext(){
	return next;
}

void Tupla::setNext(Tupla* d){
	next = d;
}

void Tupla::setIndex(int i){
	index = i;
}

void Tupla::setEndpoint(int d){
	endpoint = d;
}

void Tupla::setOrigin(int o){
	origin = o;
}