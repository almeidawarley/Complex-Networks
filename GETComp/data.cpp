#include "stdafx.h"
Data::Data(){

}

Data::~Data(){

}

void Data::setData(int i, float ar){
    id = i;
    weight = ar;
}

int Data::getId(){
    return id;
}

float Data::getWeight(){
    return weight;
}
