/**
 * \file streamstats.h
 * \brief Class to compute specific statistics over streaming data
 * \author asr
 * \version 1.0
 *
 */


#ifndef STREAMSTATS_H
#define STREAMSTATS_H

#include "streammoments.h"
#include "brent.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>


using namespace std;

/**
 *  \class StreamStats
 *	\brief Compute specific statistics over streaming data
 *	\details This class compute s* which is the best axis splitting the data into two modes and the B2 statistics
 *	which uses s* to estimates the unimodality of the distribution of the incoming data
 */
class StreamStats {
	private:
		StreamMoments standard; /*!< An object to compute moments over streaming data */
		double s_star; /*!< The "best" symmetry axis */
		StreamVar bootstrap; /*!< An object which aims to compute the variance of the symmetrized data */

	public:
		/*!
			\brief Basic constructor
			\param[in] size The window size where the statistics are computed (-1 for infinite window)
			\return A StreamStats object
		*/
		StreamStats(int size = 100);
		
		/*!
			\brief Basic constructor with a first batch of data
			\param[in] size The window size where the mean is computed (-1 for infinite window)
			\param[in] v An initial batch of data
			\param[in] center If true, remove the mean from the initial batch
			\return A StreamStats object
		*/
		StreamStats(int size, vector<double> v, bool center = false);
		
		/*!
			\brief Merge two StreamStats objects
			\details It takes half data in each StreamStats
			\param[in] S Another StreamStats instance
			\return A new StreamStats object
		*/
		StreamStats merge(const StreamStats & S) const;
		
		/*!
			\brief Return the best symmetry axis s*
		*/
		double get_s_star();
		
		/*!
			\brief Return the number of stored data
		*/
		int size();
		
		/*!
			\brief Compute and return the B2 statistics
		*/
		double B2();
		
		/*!
			\brief Compute and return the H2 statistics
		*/
		double H2();
		
		/*!
			\brief Update the statistics with a new incoming data
			\param[in] x New incoming data
			\return The state of the Ubend container (see the Ubend class)
		*/
		int step(double x);
		
		/*!
			\brief Comparison operator
			\details The means are compared
			\retval true if this->mean() < other.mean()
			\retval false if this->mean() >= other.mean()
		*/
		bool operator<(StreamStats& other);
		
		/*!
			\brief Return a copy of the underlying vector 
		*/
		vector<double> dump();
		
		/*!
			\brief Return a string which summarizes the current status of the object
			\details
		*/
		string stringStatus();

};

#endif // STREAMSTATS_H

