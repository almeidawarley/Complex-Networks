#ifndef NO_H_INCLUDED
#define NO_H_INCLUDED

class No
{
  private:
    float info; // informação real do No
    int coluna;
    No* prox;   // ponteiro para o próximo No
  public:
    No() {};
    ~No() {};

    float getInfo()         { return info; };
    No* getProx()           { return prox; };
    int getColuna()         { return coluna;};
    void setInfo(float val) { info = val; };
    void setProx(No* p)     { prox = p; };
    void setColuna(int c)   { coluna = c;};
};
#endif // NO_H_INCLUDED
