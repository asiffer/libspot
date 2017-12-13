/**
 * \file streammoments.h
 * \brief Classes to compute moments over streaming data
 * \author asr
 * \version 1.0
 *
 */




#include "ubend.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <numeric>

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
		double hm; /*!< the harmonic mean */
		
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
			\brief Return the harmonic mean within the window
		*/
		double hmean();
		
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



#ifndef STREAMVAR_H
#define STREAMVAR_H


/**
 *  \class StreamVar
 *	\brief Compute the mean and variance over streaming data
 */
class StreamVar : public StreamMean {
	protected:
		double m2; /*!< the sum of squared gaps : \sum (X_i - m)^2 */

	public:
		/*!
			\brief Basic constructor
			\param[in] size The window size where the moments are computed (-1 for infinite window)
			\return A StreamVar object
		*/
		StreamVar(int size = -1);
		
		/*!
			\brief Basic constructor with a first batch of data
			\param[in] size The window size where the moments are computed (-1 for infinite window)
			\param[in] v An initial batch of data
			\return A StreamVar object
		*/
		StreamVar(int size, vector<double> v);
		
		/*!
			\brief Return the variance within the window
		*/
		double var();
		
		/*!
			\brief Update the mean and the variance with a new incoming data
			\param[in] x_n New incoming data
			\return The state of the Ubend container (see the Ubend class)
		*/
		int step(double x_n);
};

#endif // STREAMVAR_H


#ifndef STREAMMOMENTS_H
#define STREAMMOMENTS_H


/**
 *  \class StreamMoments
 *	\brief Compute the mean, the variance and the 3th order moment over streaming data
 */
class StreamMoments : public StreamVar {
	protected:
		double m3; /*!< the sum of cubed gaps \sum (X_i - m)^3 */

	public:
		/*!
			\brief Basic constructor
			\param[in] size The window size where the moments are computed (-1 for infinite window)
			\return A StreamMoments object
		*/
		StreamMoments(int size = -1);
		
		/*!
			\brief Basic constructor with a first batch of data
			\param[in] size The window size where the moments are computed (-1 for infinite window)
			\param[in] v An initial batch of data
			\return A StreamMoments object
		*/
		StreamMoments(int size, vector<double> & v);
		
		/*!
			\brief Return the 3th order moments within the window
		*/
		double M3();
		
		/*!
			\brief Update the mean, the variance and the 3th order moment with a new incoming data
			\param[in] x_n New incoming data
			\return The state of the Ubend container (see the Ubend class)
		*/
		int step(double x_n);
};

#endif // STREAMMOMENTS_H

