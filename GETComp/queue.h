/**
	Information Diffusion in Complex Networks
	queue.h
	Purpose: defines the class Queue

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

class Queue{
    private:
        int *queue;
        int first;
        int last;
        bool *contain;
        int size;
    public:
        Queue(int s);
        ~Queue();
        bool in(int n);
        bool out();
        int next();
        bool empty();
        bool contains(int n);
};
