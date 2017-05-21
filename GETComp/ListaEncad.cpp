#include <iostream>
#include <stdlib.h>

#include "ListaEncad.h"

using namespace std;

ListaEncad::ListaEncad()
{
    //cout << "Criando uma ListaEncad" << endl;
    primeiro = NULL;
    ultimo = NULL;
    n = 0;
}

ListaEncad::~ListaEncad()
{
    //cout << "Destruindo ListaEncad" << endl;
    No* p = primeiro;
    while(p != NULL)
    {
        No* t = p->getProx();
        delete p;
        p = t;
    }
}

bool ListaEncad::busca(float val)
{
    No* p;
    for(p = primeiro; p != NULL; p = p->getProx())
    {
        if(p->getInfo() == val)
            return true;
    }
    return false;
}

void ListaEncad::inserePri(float val, int col)
{
    No* p = new No();
    p->setInfo(val);
    p->setColuna(col);
    p->setProx(primeiro);

    primeiro = p;

    n++;
    if(n==1) ultimo = p;
}

void ListaEncad::eliminaPri()
{
    No* p;
    if(primeiro != NULL)
    {
        p = primeiro;
        primeiro = p->getProx();
        delete p;

        n--;
        if(n == 0) ultimo = NULL;

    }
    else
    {
        cout << "ERRO: lista vazia" << endl;
    }
}

bool ListaEncad::vazia() {
    if(n==0){
        return true;
    }else{
        return false;
    }
}

void ListaEncad::imprime(){
    No *aux;
    aux = primeiro;
    while(aux!=NULL){
        cout << "Coluna " << aux->getColuna() << " : " << aux->getInfo() << endl;
        aux = aux->getProx();
    }
}
