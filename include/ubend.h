#include <vector>
#include <iostream>
#include <algorithm>
//#include <Python.h>

using namespace std;

#ifndef UBEND_H
#define UBEND_H

class Ubend : public std::vector<double>
{
	protected:
		int cursor;
		int capacity;
		double last_erased_data;
		
	public:
		Ubend(int N = -1);
		Ubend merge(const Ubend & U) const;
		int getCursor();
		double getLastErasedData();
		int push(double x);
		int getCapacity();
		bool isFilled();
		
};

#endif // UBEND_H
