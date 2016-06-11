#include "LItem.h"
#include "BoxMuller.h"
#include <iostream>
#include <cstdio>
#include <algorithm>
using namespace std;

//LItem *Litems[50][60000];

// compair function for function sort.

//bool cmp(LItem* a, LItem* b) {
//  return a->getValue() < b->getValue();
//}

int main() {
    FILE *ifile = fopen("dataset.txt", "r");
    float result[50];               // temp result after projecting
    int id;                         // the id of the vector and it would not be used
    int dimension;                  // the dimensions of the vector read from the file dataset.txt
    float randomVectors[50][784];
    FILE *ofile[50];                // pointer to open documents Line0-Line49
    char filename[10];
    for (int i = 0; i < 50; i++) {
        sprintf(filename, "Line%d", i);
        ofile[i] = fopen(filename, "w");
    }
    
    createVectors(randomVectors);
    //each time we read a vector and project it on 50 random vectors
    for(int i = 0; i < 60000; i++) {
        fscanf(ifile, "%d", &id);
        for(int j = 0; j < 50; j++) {
            result[j] = 0;
        }
        int count = 0;
        for(int j = 0; j  < 784; j++) {
            fscanf(ifile, "%d", &dimension);
            for(int k = 0; k < 50; k++) { 
                result[k] += dimension*randomVectors[k][j];
            }
        }
        for(int j = 0; j < 50; j++) {
            //Litems[j][i] = new LItem(id, result[j]);
            fprintf(ofile[j], "%d %f\n", id, result[j]);
        }
    }
    fclose(ifile);
    for (int i = 0; i < 50; i++) {
        fclose(ofile[i]);
    } 
    // sort in memory.
    /*for (int i = 0; i < 50; i++) {
        sort(Litems[i], Litems[i]+60000, cmp);
    }*/
    cout << endl;
}
