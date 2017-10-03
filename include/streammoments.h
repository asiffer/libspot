#include "ubend.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <Python.h>

using namespace std;



#ifndef STREAMMEAN_H
#define STREAMMEAN_H

class StreamMean : public Ubend {
	protected:
		double m; // mean

	public:
		StreamMean(int size = -1);
		StreamMean(int size, vector<double> v);
		StreamMean(const Ubend & other);
		double mean();
		int step(double x_n);
		StreamMean operator+(const StreamMean& other) const;
};

#endif // STREAMMEAN_H



#ifndef STREAMVAR_H
#define STREAMVAR_H

class StreamVar : public StreamMean {
	protected:
		double m2; // \sum (X_i - m)^2 

	public:
		StreamVar(int size = -1);
		StreamVar(int size, vector<double> v);
		double var();
		int step(double x_n);
};

#endif // STREAMVAR_H


#ifndef STREAMMOMENTS_H
#define STREAMMOMENTS_H

class StreamMoments : public StreamVar {
	protected:
		double m3; // \sum (X_i - m)^3

	public:
		StreamMoments(int size = -1);
		StreamMoments(int size, vector<double> & v);
		
		double M3();
		int step(double x_n);
};

#endif // STREAMMOMENTS_H

