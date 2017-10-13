#include "streammoments.h"
#include "brent.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
//#include <Python.h>


using namespace std;


#ifndef STREAMSTATS_H
#define STREAMSTATS_H

class StreamStats {
	private:
		StreamMoments standard;
		double s_star;
		StreamVar bootstrap;
		
		//TODO TEST
		//StreamVar axes;

	public:
		StreamStats(int size = 100);
		StreamStats(int size, vector<double> v, bool center = false);
		StreamStats merge(const StreamStats & S) const;
		
		double get_s_star();
		int size();
		double B2();
		double H2();
		
		int step(double x);
		bool operator<(StreamStats& other);
		
		vector<double> dump();
		string stringStatus();

};

#endif // STREAMSTATS_H


