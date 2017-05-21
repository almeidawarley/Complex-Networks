#include "stdafx.h"

Funct::Funct(){

}

Funct::~Funct(){

}

void Funct::wait(string text = ""){
	cout << text << " -> Enter <- " << endl;
	getchar();
}
void Funct::setInt(int* vector, int size, int value){
	for (int u = 0; u < size; u++)
		vector[u] = value;
}
void Funct::setBool(bool* vector, int size, bool value){
	for (int u = 0; u < size; u++)
		vector[u] = value;
}
void Funct::load(int current, int total, double *control){
	if ((double) current / total > *control){
		*control = *control + 0.01;
		printf("\b\b\b\b%3.f%%", *control * 100);
	}
}

/*int Funct::time(LARGE_INTEGER *t2, LARGE_INTEGER *t1, LARGE_INTEGER *f){
	return (((t2->QuadPart - t1->QuadPart) * 1000) / f->QuadPart);
}*/