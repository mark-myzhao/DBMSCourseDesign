#pragma warning(disable:4996)
#include "LItem.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <limits>

extern int a[60000][784];
extern LItem Litems[50][60000];
extern float q[100][50];
extern int queries[100][784];
extern void createVectors(float a[][784]);

// compare function for function sort.
int cmp(const void* a, const void* b) {
  float result = ((LItem*)a)->getValue() - ((LItem*)b)->getValue();
  return result < 0 ? -1 : 1;
}

void generate() {
    FILE *ifile = fopen("../data/Mnist.ds", "r");
    FILE *ifile2 = fopen("../data/Mnist.q", "r");
    float result[50];               // temp result after projecting
    int id;                         // the id of the vector and it would not be used
    float randomVectors[50][784];

    createVectors(randomVectors);
	printf("random finished!\n");
    //each time we read a vector and project it on 50 random vectors
    for (int i = 0; i < 60000; i++) {
        fscanf(ifile, "%d", &id);
		if (i % 1000 == 0)
			printf("working:%d\n", i);
		for(int j = 0; j < 50; j++) {
            result[j] = 0;
        }
        for(int j = 0; j  < 784; j++) {
            fscanf(ifile, "%d", &a[i][j]);
            for(int k = 0; k < 50; k++) { 
                result[k] += a[i][j] * randomVectors[k][j];
            }
        }
        for(int j = 0; j < 50; j++) {
            Litems[j][i].set(id, result[j]);
        }
    }
	for (int i = 0; i < 100; i++) {
		fscanf(ifile2, "%d", &id);
		printf("working2:%d\n", i);
		for (int j = 0; j < 784; j++) {
			fscanf(ifile2, "%d", &queries[i][j]);
			for (int k = 0; k < 50; k++) {
				q[i][k] += queries[i][j] * randomVectors[k][j];
			}
		}
	}
    fclose(ifile);
    fclose(ifile2);
    // sort in memory.
    for (int i = 0; i < 50; i++) {
		printf("working3:%d\n", i);
		qsort(Litems[i], 60000, sizeof(Litems[i][0]), cmp);
    }
}

//  From BoxMuller.cpp
float uniform() {
	int x;
	float y;
	x = rand();
	y = static_cast<float>(x % 100);
	y = y / 100;
	return y;
}

float uniform_fun() {
	float A = sqrt((-2)*log(uniform()));
	float B = 2 * acos(-1) * uniform();
	float C = A*cos(B);
	return C;
}


void createVectors(float a[][784]) {
	srand(static_cast<unsigned>(time(nullptr)));
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 784; j++) {
			a[i][j] = uniform_fun();
			while (a[i][j] < std::numeric_limits<int>::min() || a[i][j] > std::numeric_limits<int>::max()) {
				a[i][j] = uniform_fun();
			}
		}
	}
}


