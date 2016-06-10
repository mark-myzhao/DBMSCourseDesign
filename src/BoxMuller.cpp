#include <time.h>
#include <iomanip>
#include <math.h>
#include <fstream>
#include <limits>
#include <stdlib.h>

#define PI 3.141592653

using namespace std;

double uniform() {
	int x;
	double y;
	x = rand();
	y = (float)(x % 100);
	y = y / 100;
	return y;
}

double uniform_fun() {
	double A = sqrt((-2)*log(uniform()));
	double B = 2 * PI*uniform();
	double C = A*cos(B);
	return C;
}



void createVectors(double a[][784]) {
	srand((unsigned)time(0));
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 784; j++) {
			a[i][j] = uniform_fun();
			while (a[i][j] < std::numeric_limits<int>::min() || a[i][j] > std::numeric_limits<int>::max()) {
				a[i][j] = uniform_fun();
			}
		}
	}
	// a array for test the result
	//int test[60];
	//for(int i = 0; i < 60; i++) {
	//    test[i] = 0;
	//}

	//test code
	//for(int i = 0; i < 784; i++) {
	//    for(int j = 0; j < 784; j++) {
	//      if((int)(uniform_num[i][j]*10+30) >= 0 && (int)(uniform_num[i][j]*10+30) < 60) {
	//          test[(int)(uniform_num[i][j]*10+30)] += 1;
	//      }
	//    }
	//}
	//for(int i = 0; i < 60; i++) {
	//    cout << i << '-' << i+1 << ": ";
	//    for(int j = 0; j < test[i]; j++) {
	//        cout << '*';
	//    }
	//    cout << endl;
	//}
}
/*int main() {
double randomVectors[50][784];
createVectors(randomVectors);
for(int i = 0; i < 784; i++) {
cout << randomVectors[0][i] << ' ';
}
}*/
