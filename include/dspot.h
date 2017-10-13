#ifndef DSPOT_H
#define DSPOT_H

#include "spot.h"
#include "ubend.h"
#include "streamstats.h"
#include <vector>
#include <iostream>
#include <numeric>
//#include <Python.h>
#include <sstream>

using namespace std;



class DSpot : private Spot
{
	friend class MMSpot;
    protected:
        // Additional attributes
        int depth;
        Ubend window;
        double drift;
    
		StreamMean model;
		
		// method
		double computeDrift();
		
    public:
        // constructors
        DSpot(int d = 10);
		DSpot(int d, double q);
		DSpot(int d, double q, int n_init, double level, 
				bool up, bool down, bool alert, 
				bool bounded, int max_excess);
		
		// constructor with initial data
		DSpot(int d, double q, vector<double> init_data, bool shuffle);
		DSpot(int d, double q, vector<double> init_data, double level, 
    			bool up, bool down, bool alert, 
    			bool bounded, int max_excess, bool shuffle);
    	DSpot(const DSpot & ds);
    	
    		
		DSpot operator+(const DSpot & other) const;
		bool operator==(const DSpot & other) const;
		bool operator<(const DSpot & other) const;
		
		// overload step
		int step(double x);

		// access functions
		double getUpperThreshold();
		double getLowerThreshold();
		Bounds getThresholds();

		// bimodal test
		//double btest();

		// drift
		double getDrift();

		// overload status
		SpotStatus status();
		string stringStatus();
};

#endif // DSPOT_H
