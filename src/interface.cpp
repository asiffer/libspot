/**
	@file interface.cpp
	@brief C interface (for python notably)
	@author Alban Siffer
	@date 03 March 2018
*/

#include "dspot.h"

using namespace std;

extern "C" {
	// SPOT
	Spot * Spot_new(double q, int n_init, double level, 
			bool up, bool down, bool alert, 
			bool bounded, int max_excess) 
	{ 
		return new Spot(q, n_init, level, 
				up, down, alert, 
				bounded, max_excess);
	}
	int Spot_step(Spot * s, double x) { return s->step(x); }
	SpotStatus Spot_status(Spot * s) { return s->status(); }
	SpotConfig Spot_config(Spot * s) { return s->config(); }
	double Spot_getUpperThreshold(Spot * s) { return s->getUpperThreshold(); }
	double Spot_getLowerThreshold(Spot * s) { return s->getLowerThreshold(); }
	double Spot_getUpper_t(Spot * s) { return s->getUpper_t(); }
	double Spot_getLower_t(Spot * s) { return s->getLower_t(); }
	
	// DSPOT
	DSpot * DSpot_new(int d, double q, int n_init, double level, 
				bool up, bool down, bool alert, 
				bool bounded, int max_excess)
	{
		return new DSpot(d, q, n_init, level, 
					up, down, alert, 
					bounded, max_excess);
	}
	SpotStatus DSpot_status(DSpot * ds) { return ds->status(); }
	int DSpot_step(DSpot * ds, double x) { return ds->step(x); }
	//SpotStatus DSpot_config(DSpot * ds) { return ds->config(); }
	double DSpot_getUpperThreshold(DSpot * ds) { return ds->getUpperThreshold(); }
	double DSpot_getLowerThreshold(DSpot * ds) { return ds->getLowerThreshold(); }
	double DSpot_getUpper_t(DSpot * ds) { return ds->getUpper_t(); }
	double DSpot_getLower_t(DSpot * ds) { return ds->getLower_t(); }
	double DSpot_getDrift(DSpot * ds) { return ds->getDrift(); }
}

