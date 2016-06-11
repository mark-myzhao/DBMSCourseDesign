#include "LItem.h"
#include "BoxMuller.cpp"
#include <iostream>
#include <cstdio>
using namespace std;

LItem *Litems[50][60000];

int main() {
    FILE *ifile = fopen("dataset.txt", "r");
    float result[50];              //temp result after projecting
    int id;                         //the id of the vector and it would not be used
    int dimension;                  //the dimensions of the vector read from the file dataset.txt
	float randomVectors[50][784];
    
    
    createVectors(randomVectors);
    //each time we read a vector and project it on 50 random vectors
    for(int i = 0; i < 60000; i++) {
        fscanf(ifile, "%d", &id);
        for(int j = 0; j < 50; j++) {
            result[j] = 0;
        }
        for(int j = 0; j  < 784; j++) {
            fscanf(ifile, "%d", &dimension);
            for(int k = 0; k < 50; k++) { 
                result[k] += dimension*randomVectors[k][j];
            }
        }
        for(int j = 0; j < 50; j++) {
            Litems[j][i] = new LItem(i, result[j]);
        }
    }
    fclose(ifile);
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 50; j++) {
			cout << Litems[i][j]->getIndex() << ' ' << Litems[i][j]->getValue() << endl;
		}
	}
	cout << endl;
}
