#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define SIZEOFCOLUMNS 100
#define SIZEOFSOLUTION 35
#define NUMBEROFCRIT 6
#define NNOD 1306563

using namespace std;

void similarityBetweenSolutions(string file){
    ifstream input(file.c_str());
    ofstream output("simSolutions.txt");
    int matrix[NUMBEROFCRIT][SIZEOFSOLUTION];
    int temp;
    //reads the file
    for(int i = 0; i < NUMBEROFCRIT; i++){
        for(int j = 0; j < SIZEOFSOLUTION ; j++){
            input >> temp;
            matrix[i][j] = temp;
        }
    }
    for(int i = 0; i < NUMBEROFCRIT; i++){
        for(int j = 0; j < NUMBEROFCRIT; j++){
            //calculates similarity between columns
            int counter = 0;
            for(int e = 0; e < SIZEOFSOLUTION; e++){
                for(int f = 0; f < SIZEOFSOLUTION; f++){
                    if(matrix[i][e] == matrix[j][f]){
                        counter++;
                    }
                }
            }
            //cout << "Similarity between " << first << " and " << second << ": " << (float) counter/SIZEOFCOLUMNS << endl;
            output << (float) counter/SIZEOFSOLUTION << "; ";
        }
        output << endl;
    }

    for(int i = 0; i < NUMBEROFCRIT; i++){
        cout << "Criterio da linha " << i << ": ";
        for(int j = 0; j < SIZEOFSOLUTION; j++){
            cout << matrix[i][j] << " | ";
        }
        cout << endl;
    }
}

void similarityBetweenColumns(string file){
    ifstream input(file.c_str());
    ofstream output("simColumns.txt");
    int matrix[NUMBEROFCRIT][SIZEOFCOLUMNS];
    int temp;
    //reads the file
    for(int i = 0; i < NUMBEROFCRIT; i++){
        for(int j = 0; j < SIZEOFCOLUMNS ; j++){
            input >> temp;
            matrix[i][j] = temp;
        }
    }
    for(int i = 0; i < NUMBEROFCRIT; i++){
        for(int j = 0; j < NUMBEROFCRIT; j++){
            //calculates similarity between columns
            int counter = 0;
            for(int e = 0; e < SIZEOFCOLUMNS; e++){
                for(int f = 0; f < SIZEOFCOLUMNS; f++){
                    if(matrix[i][e] == matrix[j][f]){
                        counter++;
                    }
                }
            }
            //cout << "Similarity between " << first << " and " << second << ": " << (float) counter/SIZEOFCOLUMNS << endl;
            output << (float) counter/SIZEOFCOLUMNS << "; ";
        }
        output << endl;
    }

    //for(int i = 0; i < NUMBEROFCRIT; i++){
    //    cout << "Criterio da linha " << i << ": ";
    //    for(int j = 0; j < SIZEOFCOLUMNS; j++){
    //        cout << matrix[i][j] << " | ";
    //    }
    //    cout << endl;
    //}

}

void similarityBetweenReached(string file){
    ifstream input(file.c_str());
    ofstream output("simReached.txt");
    string buffer;
    int counter, a;
    while(input.good()){
        bool *data0 = new bool[NNOD];
        bool *data1 = new bool[NNOD];
        bool *data2 = new bool[NNOD];
        bool *data3 = new bool[NNOD];
        string msg0;
        string msg1;
        string msg2;
        string msg3;
        int counter0 = 0;
        int counter1 = 0;
        int counter2 = 0;
        int counter3 = 0;
        for(int i = 0; i < NNOD; i++){
            data0[i] = false;
            data1[i] = false;
            data2[i] = false;
            data3[i] = false;
        }
        stringstream dataS0(buffer);
        stringstream dataS1(buffer);
        stringstream dataS2(buffer);
        stringstream dataS3(buffer);

        /// Data 0
        counter = 0;
        while(counter < 52){
            getline(input, buffer, ';');
            counter++;
        }
        getline(input, buffer, ';');
        dataS0.str(buffer);
        cout << "#0] " << buffer.substr(0,10) << endl;
        getline(input, buffer, ' ');
        getline(input, msg1, ';');
        getline(input, buffer, '\n');
        while(dataS0 >> a){
            data0[a-1] = true;
            counter0++;
        }

        /// Data 1
        counter = 0;
        while(counter < 52){
            getline(input, buffer, ';');
            counter++;
        }
        getline(input, buffer, ';');
        dataS1.str(buffer);
        cout << "#1] " << buffer.substr(0,10) << endl;
        getline(input, buffer, ' ');
        getline(input, msg1, ';');
        getline(input, buffer, '\n');
        while(dataS1 >> a){
            data1[a-1] = true;
            counter1++;
        }

        /// Data 2
        counter = 0;
        while(counter < 52){
            getline(input, buffer, ';');
            counter++;
        }
        getline(input, buffer, ';');
        dataS2.str(buffer);
        cout << "#2] " << buffer.substr(0,10) << endl;
        while(dataS2 >> a){
            data2[a-1] = true;
            counter2++;
            //cout << "\t" << a  << " | " << data2[a-1] << endl;
            //getchar();
        }
        getline(input, buffer, ' ');
        getline(input, msg2, ';');
        getline(input, buffer, '\n');

        /// Data 3
        counter = 0;
        while(counter < 52){
            getline(input, buffer, ';');
            counter++;
        }
        getline(input, buffer, ';');
        dataS3.str(buffer);
        cout << "#3] " << buffer.substr(0,10) << endl;
        while(dataS3 >> a){
            data3[a-1] = true;
            counter3++;
        }
        getline(input, buffer, ' ');
        getline(input, msg3, ';');
        getline(input, buffer, '\n');

        int counter02 = 0;
        int counter13 = 0;
        for(int i = 0; i < NNOD; i++){
            counter02 += data0[i]&&data2[i];
            counter13 += data1[i]&&data3[i];
        }
        output << msg1 << ";" << msg2 << ";" << (float) counter02/counter0 << ";" << (float) counter13/counter1 <<";" << counter1/counter0 << ";"<< endl;
    }
}

int main(){
    string file;
    cout << "Nome do arquivo: "; cin >> file;
    int option;
    cout << "1] Similaridade entre colunas" << endl;
    cout << "2] Similaridade entre solucoes" << endl;
    cout << "3] Similaridade entre alcancados" << endl;
    cout << "Opcao: "; cin >> option;
    switch(option){
        case 1: similarityBetweenColumns(file); break;
        case 2: similarityBetweenSolutions(file); break;
        case 3: similarityBetweenReached(file); break;
        default: cout << "AFFER" << endl;
    }
}
