/**
	Information Diffusion in Complex Networks
	queue.cpp
	Purpose: implements the class Queue

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

Queue::Queue(int s){
    size = s;
    queue = new int[s];
    contain = new bool[s];
	for (int c = 0; c < s; c++){
		contain[c] = false;
	}
	first = 0;
    last = -1;
}

Queue::~Queue(){
    delete[] contain;
    delete[] queue;
}

bool Queue::in(int n){
	last++;
    if(n>0&&n<=size)
        contain[n-1] = true;
    else
        return false;
    queue[last] = n;
    return true;
}

bool Queue::out(){
    if(first != size)
        first++;
    else
        return false;
    return true;
}

int Queue::next(){
	if (!empty()){
		return queue[first];
	}else{
		return -1;
	}
}

bool Queue::empty(){
    if(first>last){
        return true;
    }else{
        return false;
    }
}

bool Queue::contains(int n){
    return contain[n-1];
}
