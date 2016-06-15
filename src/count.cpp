//  #pragma warning(disable:4996)
#include "LItem.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <limits>

extern int nSize;  // 60000
extern int dSize;  // 784
extern int queryNum;  // 100
extern char datasetPath[50];
extern char queryPath[50];

extern int a[60000][784];
extern LItem Litems[50][60000];
extern float q[100][50];
extern int queries[100][784];
void createVectors(float a[][784]);

// compare function for function sort.
int cmp(const void* a, const void* b) {
  int result = ((LItem*)a)->getValue() - ((LItem*)b)->getValue();
  return result < 0 ? -1 : 1;
}

void generate() {
    FILE *ifile = fopen(datasetPath, "r");
    FILE *ifile2 = fopen(queryPath, "r");
    float result[50];               // temp result after projecting
    int id;                         // the id of the vector and it would not be used
    float randomVectors[50][784];

    createVectors(randomVectors);
    //each time we read a vector and project it on 50 random vectors
    printf("projecting...\n");
    for (int i = 0; i < nSize; i++) {
        fscanf(ifile, "%d", &id);
		for(int j = 0; j < 50; j++) {
            result[j] = 0;
        }
        for(int j = 0; j  < dSize; j++) {
            fscanf(ifile, "%d", &a[i][j]);
            for(int k = 0; k < 50; k++) { 
                result[k] += a[i][j] * randomVectors[k][j];
            }
        }
        for(int j = 0; j < 50; j++) {
            Litems[j][i].set(id, result[j]);
        }
    }
	for (int i = 0; i < queryNum; i++) {
		fscanf(ifile2, "%d", &id);
		for (int j = 0; j < dSize; j++) {
			fscanf(ifile2, "%d", &queries[i][j]);
			for (int k = 0; k < 50; k++) {
				// q[i]在a[k]上的投影
				q[i][k] += queries[i][j] * randomVectors[k][j];
			}
		}
	}
	printf("projected!\n");
    fclose(ifile);
    fclose(ifile2);
    // sort in memory.
	for (int i = 0; i < 50; i++) {
		qsort(Litems[i], nSize, sizeof(Litems[i][0]), cmp);
    }
}

//  From BoxMuller.cpp
float uniform() {
	int x = rand();
	return (float)x / RAND_MAX;
}

float uniform_fun() {
	float A = sqrt((-2)*log(uniform()));
	float B = 2 * acos(-1) * uniform();
	float C = A*cos(B);
	return C;
}

void normalize(float arr[][784], int n) {
	double length = 0;
	for (int i = 0; i < dSize; ++i) {
		length += pow(arr[n][i], 2);
	}
	length = sqrt(length);
	for (int i = 0; i < dSize; ++i) {
		arr[n][i] /= length;
	}
}

void createVectors(float arr[][784]) {
	srand(static_cast<unsigned>(time(nullptr)));
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < dSize; j++) {
			arr[i][j] = uniform_fun();
			while (arr[i][j] < -std::numeric_limits<float>::max() || arr[i][j] > std::numeric_limits<float>::max()) {
				arr[i][j] = uniform_fun();
			}
		}
		normalize(arr, i);;
	}
}
