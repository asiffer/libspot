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
		int n;			/*!< Number of normal observations (not the alarms) */
		int Nt_up;		/*!< Total number of up excesses */
		int Nt_down;	/*!< Total number of down excesses */
		int ex_up;		/*!< Local number of up excesses (may be different from Nt_up in the bounded mode) */
		int ex_down;	/*!< Local number of down excesses (may be different from Nt_down in the bounded mode) */
		int al_up;		/*!< Number of up alarms */
		int al_down;	/*!< Number of down alarms */
		double t_up;	/*!< transitional up threshold */
		double t_down;	/*!< transitional down threshold */
		double z_up;	/*!< up alert thresholds */
		double z_down;	/*!< down alert thresholds */
		
		/**
			\brief Format the status to print it
		*/
		string str();
};


/**
 *  	\class SpotConfig
 *	\brief This structure gathers the configuration of a Spot object (aims to build similar instance)
 */
class SpotConfig {
	public:
		double q;		/*!< the main parameter ( P(X>z_q) < q ) */
		bool bounded; 	/*!< if true, the number of stored will be bounded by max_excess */
		int max_excess;	/*!< Maximum number of stored excesses (bounded mode) */
		bool alert;		/*!< if true, the algorithm triggers alarms (the outlier is not taking into account in the model) */
		bool up;		/*!< if true, compute upper threshold */
		bool down;		/*!< if true, compute lower threshold */
		int n_init; 	/*!< number of observation to perform calibration */
		double level; 	/*!< level of the update threshold (0<l<1) */
		
		/**
			\brief Format the config to print it
		*/
		string str();
};



/**
 *  	\class Spot
 *	\brief Embed the Spot algorithm to flag outliers in streaming data
 *	\details This object computes probabilistic upper and lower bounds (as desired) according to the incoming data and 
 *	the main parameter q
 */
class Spot
{
	protected:
	
		// SETTINGS -----------------------------------------
	
		double q; 		/*!< the main parameter ( P(X>z_q) < q ) */
	
		bool bounded; 	/*!< if true, the number of stored will be bounded by max_excess */
		
		int max_excess; /*!< Maximum number of stored excesses (bounded mode) */

		bool alert; 	/*!< if true, the algorithm triggers alarms (the outlier is not taking into account in the model) */
		
		bool up; 		/*!< if true, compute upper threshold */
		
		bool down; 		/*!< if true, compute lower threshold */

		int n_init; 	/*!< number of observation to perform calibration */
	
		double level; 	/*!< level of the update threshold (0<l<1) */
	
		// INFORMATION ------------------------------------
		
		int n; 			/*!< Number of normal observations (not the alarms) */
		
		int Nt_up; 		/*!< Number of up excesses */
		int Nt_down; 	/*!< Number of down excesses */
		
		int al_up; 		/*!< Number of up alarms */
		int al_down; 	/*!< Number of down alarms */
		
		vector<double> init_batch; /*!< initial batch (for calibration) */

		double t_up; 	/*!< transitional up threshold */
		double t_down; 	/*!< transitional down threshold */
		
		double z_up; 	/*!< up alert thresholds */
		double z_down; 	/*!< down alert thresholds */
	
		GPDfit upper_bound; 	/*!< Object to compute the upper threshold */ 
		GPDfit lower_bound; 	/*!< Object to compute the lower threshold */ 
		
		// METHODS ----------------------------------------
	
		/**
			\brief Compute threshold from the GPD fit
			\param[in] g GPDinfo (from the fit)
			\param[in] t excess quantile
			\param[in] Nt number of excesses (observations higher than t)
			\return threshold (z_q)
		*/
		double threshold(GPDinfo g, double t, int Nt);
		
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
			\param[in] q Probability of abnormal events
			\param[in] n_init Size of initial batch to perform calibration
			\return Spot object
		*/
		Spot(double q = 1e-3, int n_init = 1000);
		
		/**
			\brief Constructor with risk parametrization (q) and initial batch
			\param[in] q Probability of abnormal events
			\param[in] init_data Initial batch to perform calibration
			\return Spot object
		*/
		Spot(double q, vector<double> init_data);
		
		/**
			\brief Full parametrizable constructor
			\param[in] q Probability of abnormal events
			\param[in] n_init Number of initial observations to perform calibration
			\param[in] level Quantile level to discriminate the excesses from the normal data
			\param[in] up Compute upper threshold
			\param[in] down Compute lower threshold
			\param[in] alert Trigger alert
			\param[in] bounded Bounded mode
			\param[in] max_excess Maximum number of storable excesses (for bounded mode) 
			\return Spot object
		*/
		Spot(	double q, int n_init, double level, 
				bool up, bool down, bool alert, 
				bool bounded, int max_excess);
				
		/**
			\brief Full parametrizable constructor with initial batch
			\param[in] q Probability of abnormal events
			\param[in] init_data Initial batch to perform calibration
			\param[in] level Quantile level to discriminate the excesses from the normal data
			\param[in] up Compute upper threshold
			\param[in] down Compute lower threshold
			\param[in] alert Trigger alert
			\param[in] bounded Bounded mode
			\param[in] max_excess Maximum number of storable excesses (for bounded mode) 
			\return Spot object
		*/
		Spot(	double q, vector<double> init_data, double level, 
				bool up, bool down, bool alert, 
				bool bounded, int max_excess);
				
		/**
			\brief create a Spot object according to the configuration
			\param[in] conf a SpotConfig object
			\return Spot object
		*/
		Spot(SpotConfig conf);

		/**
			\brief Spot configuration comparison
			\param[in] spot Spot instance
			\retval true both instances have the same configuration
			\retval false instances have not the same configuration
		*/
		bool operator==(const Spot &spot) const;
	
		/**
			\brief Merge 2 Spot instances
			\param[in] spot Spot instance
			\return Spot
			\details Beware of the order. Spot_1 + Spot_2 != Spot_2 + Spot_1. 
			This function takes the lower_bound of Spot_1 and the upper_bound of Spot_2.
		*/
		Spot operator+(const Spot &spot) const;
	
	
		// SPOT iteration
	
		/**
			\brief Spot iteration
			\param[in] v input data
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
			\param[in] v input data
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
	
		// config of the algorithm
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
};


#endif // SPOT_H

