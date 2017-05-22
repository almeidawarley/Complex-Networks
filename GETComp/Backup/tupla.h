#include "stdafx.h"

using namespace std;

class Tupla{
private:
	int index, origin, endpoint;
	Tupla* next;
public:
	Tupla(int i, int o, int d);
	~Tupla();
	int getIndex();
	int getOrigin();
	int getEndpoint();
	void setIndex(int i);
	void setOrigin(int o);
	void setEndpoint(int d);
	Tupla* getNext();
	void setNext(Tupla* t);

};