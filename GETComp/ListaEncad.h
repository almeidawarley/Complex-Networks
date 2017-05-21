#ifndef LISTASENCAD_H_INCLUDED
#define LISTASENCAD_H_INCLUDED

#include "No.h"

class ListaEncad
{
  public:
    ListaEncad();
    ~ListaEncad();
    bool busca(float val);
    void inserePri(float val, int col);
    void eliminaPri();
    bool vazia();
    void imprime();

  private:
    No* primeiro;
    No* ultimo;
    int n;
};
#endif
