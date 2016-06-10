#include <fstream>
#include <iostream>
using namespace std;

class LItem {
    public:
        LItem(int index = -1, double value = -1.0): index_(index), value_(value) {};
		void set(int index, double value);
		int getIndex();
		double getValue();
    private:
        int index_;
        double value_;
};
