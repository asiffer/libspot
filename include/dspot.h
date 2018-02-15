/**
	\file dspot.h
	\brief Implement the DSpot algorithm
	\details It flags outliers in streaming data (with drift!)
	\author asr
	\version 1.0
*/



#include "spot.h"
#include "ubend.h"
//#include "streamstats.h"
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
		double m; /*!< the classic mean */
		
	public:
		/*!
			\brief Basic constructor
			\param[in] size The window size where the mean is computed (-1 for infinite window)
			\return A StreamMean object
		*/
		StreamMean(int size = -1);
		
		/*!
			\brief Basic constructor with a first batch of data
			\param[in] size The window size where the mean is computed (-1 for infinite window)
			\param[in] v An initial batch of data
			\return A StreamMean object
		*/
		StreamMean(int size, vector<double> v);
		
		/*!
			\brief Constructor from a Ubend object
			\param[in] other External Ubend object
			\return A StreamMean object
		*/
		StreamMean(const Ubend & other);
		
		/*!
			\brief Return the mean within the window
		*/
		double mean();
		
		
		/*!
			\brief Update the mean with a new incoming data
			\param[in] x_n New incoming data
			\return The state of the Ubend container (see the Ubend class)
		*/
		int step(double x_n);
		
		/*!
			\brief The sum operator (merge the Ubend)
			\param[in] other Another StreamMean object
			\return A new StreamMean instance
		*/		
		StreamMean operator+(const StreamMean& other) const;
};

#endif // STREAMMEAN_H




#ifndef DSPOT_H
#define DSPOT_H

/**
 *  \class DSpot
 *	\brief Embed the DSpot algorithm to flag outliers in streaming and drifting data
 *	\details This object computes probabilistic upper and lower bounds (as desired) according to the incoming data, the main parameter q and the local drift (a moving average with a given depth)
 */
class DSpot : public Spot
{
    protected:
        // Additional attributes
        
        int depth;			/*!< the depth of the moving average */
        double drift;		/*!< the local drift */
    
		StreamMean model;	/*!< A buffer to compute the local drift */
		
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
			\param[in] d Depth of the moving average
			\param[in] q Probability of abnormal events
			\param[in] n_init Size of initial batch to perform calibration
			\return Spot object
		*/
		DSpot(int d = 10, double q = 1e-3, int n_init = 1000);
		
		/**
			\brief Constructor with risk parametrization (q) and initial batch
			\param[in] d Depth of the moving average
			\param[in] q Probability of abnormal events
			\param[in] init_data Initial batch to perform calibration
			\return Spot object
		*/
		DSpot(int d, double q, vector<double> init_data);
		
		/**
			\brief Full parametrizable constructor
			\param[in] d Depth of the moving average
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
		DSpot(	int d, double q, int n_init, double level, 
				bool up, bool down, bool alert, bool bounded, int max_excess);
        
		/**
			\brief Full parametrizable constructor
			\param[in] d Depth of the moving average
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
        DSpot(	int d, double q, vector<double> init_data, double level, 
				bool up, bool down, bool alert, bool bounded, int max_excess); 
        
		/**
			\brief Default constructor
			\param[in] d The depth of the local model
			\param[in] args Spot arguments (see Spot class)
			\return DSpot object
		*/
        //template<typename... Args> 
        //DSpot(int d = 10, Args&... args) : Spot(args...) {this->depth = d;}
        
        /**
			\brief copy constructor (copy only the parameters)
			\return DSpot object
		*/
    	//DSpot(const DSpot & ds);
    	
    		
		/**
			\brief DSpot configuration comparison
			\param[in] other DSpot instance
			\retval true both instances have the same configuration
			\retval false instances have not the same configuration
		*/
		bool operator==(const DSpot & other) const;
		
		/**
			\brief DSpot bounds comparison
			\param[in] other DSpot instance
			\retval true if this->z_up + this->drift < other.z_down + other.drift
			\retval false otherwise
		*/
		bool operator<(const DSpot & other) const;
		
		// overload step
		
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
		int step(double x);

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

		// overload status
		
		/**
			\brief Get the internal state of the DSpot instance
		*/
		SpotStatus status();
		
		/**
			\brief Format the config to print it
		*/
		string stringStatus();
};

#endif // DSPOT_H
