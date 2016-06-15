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
extern int randomSize;
extern char datasetPath[50];
extern char queryPath[50];

//extern int a[60000][784];
extern int **a;
extern LItem **Litems;
extern float **q;
extern int **queries;

// compare function for function sort.
int cmp(const void* a, const void* b) {
  int result = ((LItem*)a)->getValue() - ((LItem*)b)->getValue();
  return result < 0 ? -1 : 1;
}

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


void generate() {
    FILE *ifile = fopen(datasetPath, "r");
    FILE *ifile2 = fopen(queryPath, "r");
    float result[randomSize];               // temp result after projecting
    int id;                         // the id of the vector and it would not be used
    float randomVectors[randomSize][dSize];
    srand(static_cast<unsigned>(time(nullptr)));
	for (int i = 0; i < randomSize; i++) {
		for (int j = 0; j < dSize; j++) {
			randomVectors[i][j] = uniform_fun();
			while (randomVectors[i][j] < -std::numeric_limits<float>::max() || randomVectors[i][j] > std::numeric_limits<float>::max()) {
				randomVectors[i][j] = uniform_fun();
			}
		}
		double length = 0;
		for (int j = 0; j < dSize; ++j) {
			length += pow(randomVectors[i][j], 2);
		}
		length = sqrt(length);
		for (int j = 0; j < dSize; ++j) {
			randomVectors[i][j] /= length;
		}
	}
    //each time we read a vector and project it on 50 random vectors
    printf("projecting...\n");
    for (int i = 0; i < nSize; i++) {
        fscanf(ifile, "%d", &id);
		for(int j = 0; j < randomSize; j++) {
            result[j] = 0;
        }
        for(int j = 0; j  < dSize; j++) {
            fscanf(ifile, "%d", &a[i][j]);
            for(int k = 0; k < randomSize; k++) { 
                result[k] += a[i][j] * randomVectors[k][j];
            }
        }
        for(int j = 0; j < randomSize; j++) {
            Litems[j][i].set(id, result[j]);
        }
    }
	for (int i = 0; i < queryNum; i++) {
		fscanf(ifile2, "%d", &id);
		for (int j = 0; j < dSize; j++) {
			fscanf(ifile2, "%d", &queries[i][j]);
			for (int k = 0; k < randomSize; k++) {
				q[i][k] += queries[i][j] * randomVectors[k][j];
			}
		}
	}
	printf("projected!\n");
    fclose(ifile);
    fclose(ifile2);
    // sort in memory.
	for (int i = 0; i < randomSize; i++) {
		qsort(Litems[i], nSize, sizeof(Litems[i][0]), cmp);
    }
}
