/**
	\file spot.h
	\brief Implement the Spot algorithm
	\details It flags outliers in streaming data
	\author asr
*/



#include "gpdfit.h"
#include "bounds.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;


#ifndef SPOT_H
#define SPOT_H




/** 
 * 	\enum SPOTEVENT
 * 	\brief events returned by the (D)SPOT algorithm
 *
 * 	The previous integer values we used are kept
 */
enum SPOTEVENT: int {
	NORMAL = 0,
	ALERT_UP = 1,
	ALERT_DOWN = -1,
	EXCESS_UP = 2,
	EXCESS_DOWN = -2,
	INIT_BATCH = 3,
	CALIBRATION = 4
};


/**
 *  \class SpotStatus
 *	\brief This structure summarizes the status of the Spot object around the decision area
 *	\details It gives the number of excesses, the number of alarms, the number of normal observations, the values of the thresholds etc.
 */
class SpotStatus {
	public:
		/** Number of normal observations (not the alarms) */
		int n; 			
		
		/** Current number of up excesses */
		int ex_up;
		/** Current number of down excesses */ 		
		int ex_down; 		
		
		/** Total number of up excesses */
		int Nt_up;
		/** Total number of down excesses */ 		
		int Nt_down; 
		
		/** Number of up alarms */
		int al_up; 	
		/** Number of down alarms */	
		int al_down; 		

		/** transitional up threshold */
		double t_up; 	
		/** transitional down threshold */	
		double t_down; 		
		
		/** up alert thresholds */
		double z_up; 		
		/** down alert thresholds */
		double z_down; 	
		
		/**
			\brief Format the status to print it
		*/
		string str();
};


/**
 *  \class SpotConfig
 *	\brief This structure gathers the configuration of a Spot object (aims to build similar instance)
 */
class SpotConfig {
	public:
		/** the main parameter ( P(X>z_q) < q ) */
		double q; 
	
		/** if true, the number of stored will be bounded by max_excess */
		bool bounded; 		
		
		/** Maximum number of stored excesses (bounded mode) */
		int max_excess; 	

		/** if true, the algorithm triggers alarms (the outlier is not taking into account in the model) */
		bool alert; 		
		
		/** if true, compute upper threshold */
		bool up; 		
		
		/** if true, compute lower threshold */
		bool down; 		

		/** number of observation to perform calibration */
		int n_init; 		
	
		/** level of the update threshold (0<l<1) */
		double level; 
		
		/**
			\brief Format the config to print it
		*/
		string str();
};



/**
 *  \class Spot
 *	\brief Embed the Spot algorithm to flag outliers in streaming data
 *	\details This object computes probabilistic upper and lower bounds (as desired) according to the incoming data and 
 *	the main parameter q
 */
class Spot
{
	protected:
	
		// SETTINGS -----------------------------------------
	
		/** the main parameter ( P(X>z_q) < q ) */
		double q;
	
		/** if true, the number of stored will be bounded by max_excess */
		bool bounded; 		
		
		/** Maximum number of stored excesses (bounded mode) */
		int max_excess; 	

		/** if true, the algorithm triggers alarms (the outlier is not taking into account in the model) */
		bool alert; 		
		
		/** if true, compute upper threshold */
		bool up; 		
		
		/** if true, compute lower threshold */
		bool down; 		

		/** number of observation to perform calibration */
		int n_init; 		
	
		/** level of the update threshold (0<l<1) */
		double level; 		
	
		// INFORMATION ------------------------------------
		
		/** Number of normal observations (not the alarms) */
		int n; 			
		
		/** Number of up excesses */
		int Nt_up;
		/** Number of down excesses */ 		
		int Nt_down; 		
		
		/** Number of up alarms */
		int al_up; 	
		/** Number of down alarms */	
		int al_down; 		
		
		/** initial batch (for calibration) */
		vector<double> init_batch; 

		/** transitional up threshold */
		double t_up; 	
		/** transitional down threshold */	
		double t_down; 		
		
		/** up alert thresholds */
		double z_up; 		
		/** down alert thresholds */
		double z_down; 		
	
		/** Object to compute the upper threshold */ 
		GPDfit upper_bound; 	
		/** Object to compute the lower threshold */
		GPDfit lower_bound; 	
		
		
		// METHODS ----------------------------------------
	
		
		/**
			\brief GPD fit for the upper bound (update upper threshold)
		*/
		void fitup();
		
		/**
			\brief GPD fit for the lower bound (update upper threshold)
		*/
		void fitdown();


	
	public:
		// Constructors
	
		/**
			\brief Default constructor
			\param q Probability of abnormal events
			\param n_init Size of initial batch to perform calibration
			\return Spot object
		*/
		Spot(double q = 1e-3, int n_init = 1000);
		
		/**
			\brief Constructor with risk parametrization (q) and initial batch
			\param q Probability of abnormal events
			\param init_data Initial batch to perform calibration
			\return Spot object
		*/
		Spot(double q, vector<double> init_data);
		
		/**
			\brief Full parametrizable constructor
			\param q Probability of abnormal events
			\param n_init Number of initial observations to perform calibration
			\param level Quantile level to discriminate the excesses from the normal data
			\param up Compute upper threshold
			\param down Compute lower threshold
			\param alert Trigger alert
			\param bounded Bounded mode
			\param max_excess Maximum number of storable excesses (for bounded mode) 
			\return Spot object
		*/
		Spot(	double q, int n_init, double level, 
				bool up, bool down, bool alert, 
				bool bounded, int max_excess);
				
		/**
			\brief Full parametrizable constructor with initial batch
			\param q Probability of abnormal events
			\param init_data Initial batch to perform calibration
			\param level Quantile level to discriminate the excesses from the normal data
			\param up Compute upper threshold
			\param down Compute lower threshold
			\param alert Trigger alert
			\param bounded Bounded mode
			\param max_excess Maximum number of storable excesses (for bounded mode) 
			\return Spot object
		*/
		Spot(	double q, vector<double> init_data, double level, 
				bool up, bool down, bool alert, 
				bool bounded, int max_excess);
				
		/**
			\brief create a Spot object according to the configuration
			\param conf a SpotConfig object
			\return Spot object
		*/
		Spot(SpotConfig conf);

		/**
			\brief Spot configuration comparison
			\param spot Spot instance
			\retval true both instances have the same configuration
			\retval false instances have not the same configuration
		*/
		bool operator==(const Spot &spot) const;
	
		/**
			\brief Merge 2 Spot instances
			\param spot Spot instance
			\return Spot
			\details Beware of the order. Spot_1 + Spot_2 != Spot_2 + Spot_1. 
			This function takes the lower_bound of Spot_1 and the upper_bound of Spot_2.
		*/
		Spot operator+(const Spot &spot) const;
	
	
		// SPOT iteration
	
		/**
			\brief Spot iteration
			\param v input data
			\return The nature of the input data
			\retval 0 Normal data
			\retval	1 Up alert
			\retval	-1 Down alert
			\retval	2 Up excess
			\retval	-2 Down excess
			\retval	3 to initial batch
			\retval	4 calibration step
		*/
		/*
		int step(double v);
		*/


		/**
			\brief Spot iteration
			\param v input data
			\return The nature of the input data
		*/
		SPOTEVENT step(double v);
	

		/**
			\brief Perform Spot initial calibration (after the first n_init observations)
		*/
		void calibrate();
	
		/**
			\brief Get the internal state of the Spot instance
		*/
		SpotStatus status();
	
		/**
			\brief Get the internal state of the Spot instance (display-ready)
		*/
		string stringStatus();
	
		/**
			\brief Return the current state of the Spot instance through a single line string
			\param log_level 0,1 or 2 according to the desired verbosity
		*/
		string log(int log_level);
		

		/**
			\brief Return the configuration of the Spot instance
		*/
		SpotConfig config() const;
	
		// access functions

		/**
			\brief Get the current upper threshold
		*/
		double getUpperThreshold();
	
		/**
			\brief Get the current lower threshold
		*/
		double getLowerThreshold();
	
		/**
			\brief Get the current thresholds (through a Bounds structure)
		*/
		Bounds getThresholds();
	
		/**
			\brief Get the upper excess quantile
		*/
		double getUpper_t();
	
		/**
			\brief Get the lower excess quantile
		*/
		double getLower_t();
		
		/**
			\brief Set the risk parameter q
		*/
		void set_q(double q_new);
		
		/**
			\brief Give the probability to observe things higher than a value
			\param z input value
			\return proability 1-F(z)
		*/
		double up_probability(double z);
		
		/**
			\brief Give the probability to observe things lower than a value
			\param z input value
			\return proability F(z)
		*/
		double down_probability(double z);
};


#endif // SPOT_H

