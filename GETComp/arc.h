/**
	Information Diffusion in Complex Networks
	arc.h
	Purpose: defines the class Arc

	@author Warley Almeida Silva
	@version 1.0
*/

#include "stdafx.h"

class Arc{
    private:
        float weight;
		int weightS;
    public:
        Arc();
		~Arc();
		int endpoint;
        void setEndpoint(int d);
        void setWeight(float w);
        int getEndpoint();
        float getWeight();
};

