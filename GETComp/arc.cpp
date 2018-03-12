/**
	Information Diffusion in Complex Networks
	arc.cpp
	Purpose: implements the class Arc

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

Arc::Arc(){
    endpoint = -1;
    weight = -1;
}

Arc::~Arc(){

}

void Arc::setEndpoint(int d){
    endpoint = d;
}

void Arc::setWeight(float w){
    weight = w;
}

int Arc::getEndpoint(){
    return endpoint;
}

float Arc::getWeight(){
    return weight;
}