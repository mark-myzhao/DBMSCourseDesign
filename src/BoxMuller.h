#include <cstdlib>
#include <cmath>
#include <ctime>
#include <limits>
#include "def.h"

using namespace std;

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
	float B = 2 * PI*uniform();
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