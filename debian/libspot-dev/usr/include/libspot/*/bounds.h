/**
	\file bounds.h
	\brief A basic structure to manipulate bounds
	\author asr
	\version 1.0
*/

#include <vector>

using namespace std;


#ifndef BOUNDS_H
#define BOUNDS_H


/**
 *  \class Bounds
 *	\brief A structure which embeds upper and lower thresholds
 *	\details Some basic methods are also implemented
 *	------|------------|-------
 *		 down		   up
 */
class Bounds {
	public:
		double down; /*!< lower threshold */
		double up; /*!< upper threshold */
	
		/**
			\brief Basic constructor
			\param[in] d the value of the lower bounds
			\param[in] u the value of the upper bounds
			\return GPDfit object
		*/
		Bounds(double d = 0.0, double u = 0.0);
		
		/**
			\brief Compute the distance between a value x and the interval [down,up]
			\details If x belongs to [down,up], it returns 0. Otherwises it returns d(x) = min(|x-down|,|x-up|)
			\param[in] x a value
			\return the distance d(x)
		*/
		double distanceTo(double x);
		
		/**
			\brief Check if a value x belongs to the interval [down,up]
			\retval true x belongs to the interval [down,up]
			\retval false x does not belong to the interval [down,up]
		*/
		bool contains(double x);
		
		/**
			\brief Compare this->up and other.down
			\retval true if this->up < other.down
			\retval false otherwise
		*/
		bool operator<(Bounds const& other) const;
		
		/**
			\brief Compare this->down and other.up
			\retval true if this->down > other.up
			\retval false otherwise
		*/
		bool operator>(Bounds const& other) const;
		
};

#endif // BOUNDS_H
