/**
	\file dspot.h
	\brief Implement the DSpot algorithm
	\details It flags outliers in streaming data (with drift!)
	\author asr
*/



#include "spot.h"
#include "ubend.h"
#include <vector>
#include <iostream>
#include <numeric>
#include <sstream>

using namespace std;




#ifndef STREAMMEAN_H
#define STREAMMEAN_H

/**
 *  \class StreamMean
 *	\brief Compute the mean over streaming data (window)
 *	\details The harmonic mean has also been added but it is not used
 */
class StreamMean : public Ubend {
	protected:
		/** the classic mean */
		double m; 
		
	public:
		/**
			\brief Basic constructor
			\param size The window size where the mean is computed (-1 for infinite window)
			\return A StreamMean object
		*/
		StreamMean(int size = -1);
		
		/**
			\brief Basic constructor with a first batch of data
			\param size The window size where the mean is computed (-1 for infinite window)
			\param v An initial batch of data
			\return A StreamMean object
		*/
		StreamMean(int size, vector<double> v);
		
		/**
			\brief Constructor from a Ubend object
			\param other External Ubend object
			\return A StreamMean object
		*/
		StreamMean(const Ubend & other);
		
		/**
			\brief Return the mean within the window
		*/
		double mean();
		
		
		/**
			\brief Update the mean with a new incoming data
			\param x_n New incoming data
			\return The state of the Ubend container (see the Ubend class)
		*/
		UBENDSTATUS step(double x_n);
		
		/**
			\brief The sum operator (merge the Ubend)
			\param other Another StreamMean object
			\return A new StreamMean instance
		*/		
		StreamMean operator+(const StreamMean& other) const;
};

#endif // STREAMMEAN_H




#ifndef DSPOT_H
#define DSPOT_H


/**
 *  \class DSpotStatus
 *	\brief This structure summarizes the status of the DSpot object around the decision area (inherit SpotStatus)
 *	\details It gives the number of excesses, the number of alarms, the number of normal observations, the values of the thresholds etc.
 */
class DSpotStatus : public SpotStatus{
	public:
		/** the local drift */	
		double drift;
		
		/**
			\brief Format the status to print it
		*/
		string str();

};



/**
 *  \class DSpotConfig
 *	\brief This structure gathers the configuration of a DSpot object (aims to build similar instance)
 *	\details inherit SpotConfig
 */
class DSpotConfig: public SpotConfig {
	public:
		/** the depth of the moving average */
        int depth;
        	
        /**
			\brief Format the config to print it
		*/
		string str();
};



/**
 *  \class DSpot
 *	\brief Embed the DSpot algorithm to flag outliers in streaming and drifting data
 *	\details This object computes probabilistic upper and lower bounds (as desired) according to the incoming data, the main parameter q and the local drift (a moving average with a given depth)
 */
class DSpot : public Spot
{
    protected:
        // Additional attributes
        /** the depth of the moving average */
        int depth;		
        
        /** the local drift */	
        double drift;	
 
	/** A buffer to compute the local drift */   
	StreamMean model;
	
	// method
	
	/**
		\brief Compute and return the local drift
		\details the attribute "drift" is updated
	*/
	double computeDrift();
		
    public:
        // constructors (use the Spot constructor syntax)
        
        	/**
			\brief Default constructor
			\param d Depth of the moving average
			\param q Probability of abnormal events
			\param n_init Size of initial batch to perform calibration
			\return Spot object
		*/
		DSpot(int d = 10, double q = 1e-3, int n_init = 1000);
		
		/**
			\brief Constructor with risk parametrization (q) and initial batch
			\param d Depth of the moving average
			\param q Probability of abnormal events
			\param init_data Initial batch to perform calibration
			\return Spot object
		*/
		DSpot(int d, double q, vector<double> init_data);
		
		/**
			\brief Full parametrizable constructor
			\param d Depth of the moving average
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
		DSpot(	int d, double q, int n_init, double level, 
				bool up, bool down, bool alert, bool bounded, int max_excess);
        
        /** EXPERIMENTAL -> to avoid boolean export
         */
        DSpot(	int d, double q, int n_init, double level, 
				int up, int down, int alert, int bounded, int max_excess);
        
		/**
			\brief Full parametrizable constructor
			\param d Depth of the moving average
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
        	DSpot(	int d, double q, vector<double> init_data, double level, 
				bool up, bool down, bool alert, bool bounded, int max_excess); 
        
		/**
			\brief Default constructor
			\param d The depth of the local model
			\param args Spot arguments (see Spot class)
			\return DSpot object
		*/
        //	template<typename... Args> 
        //	DSpot(int d = 10, Args&... args) : Spot(args...) {this->depth = d;}
        

    	
    		
		/**
			\brief DSpot configuration comparison
			\param other DSpot instance
			\retval true both instances have the same configuration
			\retval false instances have not the same configuration
		*/
		bool operator==(const DSpot & other) const;
		
		/**
			\brief DSpot bounds comparison
			\param other DSpot instance
			\retval true if this->z_up + this->drift < other.z_down + other.drift
			\retval false otherwise
		*/
		bool operator<(const DSpot & other) const;
		
		// overload step
		
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
		int step(double x);*/
		
		
		/**
			\brief Spot iteration
			\param v input data
			\return The nature of the input data
		*/
		SPOTEVENT step(double x);

		// access functions
		
		/**
			\brief Return the absolute upper threshold (with the drift)
		*/
		double getUpperThreshold();

		/**
			\brief Return the absolute lower threshold (with the drift)
		*/
		double getLowerThreshold();

		/**
			\brief Return the absolute thresholds (with the drift)
		*/
		Bounds getThresholds();

		// drift

		/**
			\brief Return the local drift
		*/
		double getDrift();

		/**
			@brief Return the current state of the DSpot instance through a single line string
		*/
		// overload log
		string log(int log_level);

		// overload status
		
		/**
			\brief Get the internal state of the DSpot instance
			\details overload Spot method
		*/
		DSpotStatus status();
		
		/**
			\brief Return the configuration of the DSpot instance
			\details overload Spot method
		*/
		DSpotConfig config() const;
		
		/**
			\brief Format the status to print it
		*/
		string stringStatus();
		
		
		/**
			\brief Get the upper excess quantile
			\details Overload Spot method (return the real absolute value with the drift)
		*/
		double getUpper_t();
	
		/**
			\brief Get the lower excess quantile
			\details Overload Spot method (return the real absolute value with the drift)
		*/
		double getLower_t();
		
		
		/**
			\brief Give the probability to observe things higher than a value
			\details Overload Spot method (return the real absolute value with the drift)
			\param z input value
			\return proability 1-F(z)
		*/
		double up_probability(double z);
		
		/**
			\brief Give the probability to observe things lower than a value
			\details Overload Spot method (return the real absolute value with the drift)
			\param z input value
			\return proability F(z)
		*/
		double down_probability(double z);
};

#endif // DSPOT_H
