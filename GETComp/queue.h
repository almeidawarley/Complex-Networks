#include <iostream>

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
