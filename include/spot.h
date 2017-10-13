#ifndef SPOT_H
#define SPOT_H

#include "gpdfit.h"
#include "bounds.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
//#include <Python.h>
#include <sstream>

using namespace std;


class SpotInfo {
	public:
		GPDinfo info;
		double z;
		int Nt;
};



class SpotStatus {
	public:
		int n;
		int Nt_up;
		int Nt_down;
		int ex_up;
		int ex_down;
		int al_up;
		int al_down;
		double t_up;
		double t_down;
		double z_up;
		double z_down;
		string str();
};



class SpotConfig {
	public:
		double q;
		bool bounded;
		int max_excess;
		bool alert;
		bool up;
		bool down;
		int n_init;
		double level;
		string str();
};




class Spot
{

protected:
	
	// SETTINGS -----------------------------------------
	
	// the main parameter ( P(X>z_q) < q )
    double q;
	
	// bounded mode
	bool bounded;
    
    // (bounded mode) Maximum number of stored excesses
    int max_excess;

	// Trigger alert
    bool alert;
    
    // Compute upper threshold
    bool up;
    
    // Compute lower threshold
    bool down;

	// number of observation to perform calibration
	int n_init;
	
	// level of the update threshold (l)
	double level;
	
	// INFORMATION ------------------------------------
    
    // Number of normal observations (not the alarms)
    int n;
    
    // Number of excesses
    int Nt_up;
    int Nt_down;
    
    // Number of alerts
    int al_up;
    int al_down;
    
    // Initial batch (for calibration)
    vector<double> init_batch;

	// Update thresholds
    double t_up;
    double t_down;
    
    // Alert thresholds
    double z_up;
    double z_down;
	
	// Structures to compute the thresholds (excesses)
    GPDfit upper_bound;
    GPDfit lower_bound;
    
	// METHODS ----------------------------------------
    double threshold(GPDinfo g, double t, int Nt);
    void fitup();
    void fitdown();


	
public:

	// Constructors
    Spot();
    Spot(double q);
    Spot(double q, int n_init);
    Spot(double q, vector<double> init_data);
    Spot(	double q, int n_init, double level, 
    		bool up, bool down, bool alert, 
    		bool bounded, int max_excess);
    Spot(	double q, vector<double> init_data, double level, 
    		bool up, bool down, bool alert, 
    		bool bounded, int max_excess);
    Spot(SpotConfig conf);
    Spot copy() const;
    
    //NOT TODO COPY CONSTRUCTOR
    //Spot(const Spot &);


	bool operator==(const Spot &spot) const;
	Spot operator+(const Spot &spot) const;
	
	//Spot addWithDrift(const Spot &other, double my_drift, double other_drift) const;
	
	// SPOT iteration
	int step(double v);
	
	// calibration
	void calibrate();
	
	// status of the algorithm
	SpotStatus status();
	string stringStatus();
	
	// config of the algorithm
	SpotConfig config();
	
	// access functions
	double getUpperThreshold();
	double getLowerThreshold();
	Bounds getThresholds();
	
	double getUpper_t();
	double getLower_t();
};

#endif // SPOT_H
