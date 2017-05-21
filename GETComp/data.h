#ifndef DATA_H
#define DATA_H

class Data
{
    public:
        Data();
        ~Data();
        int getId();
        float getWeight();
        void setData(int i, float ar);

    private:
        int id;
        float weight;
};

#endif // DATA_H
