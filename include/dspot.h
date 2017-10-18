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
    protected:
        // Additional attributes
        int depth;
        Ubend window;
        double drift;
    
		StreamMean model;
		
		// method
		double computeDrift();
		
    public:
        // constructors (use the Spot constructor syntax)
        template<typename... Args> DSpot(int d = 10, Args&... args) : Spot(args...) {this->depth = d;}
    	DSpot(const DSpot & ds);
    	
    		
		//DSpot operator+(const DSpot & other) const;
		bool operator==(const DSpot & other) const;
		bool operator<(const DSpot & other) const;
		
		// overload step
		int step(double x);

		// access functions
		double getUpperThreshold();
		double getLowerThreshold();
		Bounds getThresholds();

		// drift
		double getDrift();

		// overload status
		SpotStatus status();
		string stringStatus();
};

#endif // DSPOT_H
