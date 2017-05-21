#ifndef MAXHEAP_H
#define MAXHEAP_H
#include "Data.h"

class MaxHeap{

    public:
        MaxHeap(int mSize);
        ~MaxHeap();

        bool add(int i, float ar);
        int extract();

        int parent(int i);
        int child(int i);
		void siftUp();
		void siftDown();

        void print();
		bool empty();
		int size();

		void sort(list<int> *set, int stop = -1);

    private:
        Data **data;
        int maxSize;
        int lastIndex;
};

#endif // MAXHEAP_H
