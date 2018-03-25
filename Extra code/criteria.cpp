/**
	Information Diffusion in Complex Networks
	criteria.cpp
	Purpose: calculates four criterias based on raw information

	@author Warley Almeida Silva
	@version 1.0
*/
#include <iostream>
#include <fstream>
#include <math.h>
#define INFINITE 999999999

int main(){
    //std::ifstream input("eccentricity.txt");
    std::ifstream input("pathAtAllNodes.txt");
    std::ofstream output("criterio99.txt");

    int a,b,n;
    float c;
    input >> n;
    output << n << '\n';
    while(input >> a >> b){
        if (b==0)
            b = INFINITE;
        c = float(1/float(b))*pow(10,7); // criteria 3
        //c = float(float(n-1)/float(b)); // criteria 4
        //c = float(1/float(b)); // criteria 5
        //c = float(float(23*n - b)/float(n-1)); // criteria 6
        output << a << ' ' << c << '\n';
    }
}
