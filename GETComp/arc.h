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

