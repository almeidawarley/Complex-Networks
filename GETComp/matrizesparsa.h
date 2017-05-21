#include <iostream>
#include "ListaEncad.h"

using namespace std;

class MatrizEsparsa{
    private:
        ListaEncad **lista = NULL;
        int tamanho;
    public:
        MatrizEsparsa(int dim);
        ~MatrizEsparsa();
        void insere(int val, int col, int lin);
        void imprime();

};
