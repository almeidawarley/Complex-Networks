#include <iostream>
#include "matrizesparsa.h"

using namespace std;

MatrizEsparsa::MatrizEsparsa(int dim){
    int i;
    lista = new ListaEncad* [dim];
    for(i=0; i<dim; i++){
        lista[i] = new ListaEncad();
    }
    tamanho = dim;
}

MatrizEsparsa::~MatrizEsparsa(){
    int t;
    for(t=0; t<tamanho; t++){
        delete lista[t];
    }
    delete lista;
}

void MatrizEsparsa::imprime(){
    int t, i;
    No *aux;
    for(t=0; t<tamanho; t++){
        if(!lista[t]->vazia()){
            cout << "Linha " << t << " : " << endl;
            lista[t]->imprime();
        }
    }
}

void MatrizEsparsa::insere(int val, int col, int lin){
    int t, i;
    if(col>=tamanho||lin>=tamanho){
        cout << "Valores de linha e coluna digitados sao invalidos" << endl;
    }else{
        lista[lin]->inserePri(val, col);
    }
}
