/**
	\file ubend.h
	\brief A "ubend" container class.
	\details This is a kind of circular vector.
	\author asr
*/

#ifndef UBEND_H
#define UBEND_H

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;


/** 
 * 	\enum UBENDSTATUS
 * 	\brief Status of the Ubend container
 *
 * 	The previous integer values we used are kept
 */
enum UBENDSTATUS: int {
	CRUISING = 1,
	JUST_FILLED = 0,
	NOT_FILLED = -1,
	INFINITE = -2
};


/**
 *  	\class Ubend
 *	\brief This container is a kind of circular vector. 
 *	\details First it is empty. 
 *	Then we feed it with data until its max capacity is reached (transitory state).
 *	When it is full, the earlier data erase the older one (cruise state).
 */
class Ubend : public std::vector<double>
{
	protected:
		/** position of the next block to fill */
		int cursor; 			
		
		/** maximum size of the container */
		int capacity; 			
		
		/** last value erased (replaced by a new one) */
		double last_erased_data; 	
		
	public:
		/**
			\brief Basic Ubend constructor
			\param[in] N the maximum size of the container (capacity)
			\return a Ubend object
		*/
		Ubend(int N = -1);
		
		/**
			\brief Merge two Ubend instances (take half data in each instance)
			\return A new Ubend object
		*/
		Ubend merge(const Ubend & U) const;
		
		/**
			\brief Get the value of the cursor, so the next block to fill
			\return the current cursor
		*/
		int getCursor();
		
		/**
			\brief Get the value of the last erased data
			\return the last erased data
		*/
		double getLastErasedData();
		
		/**
			\brief Get the capacity of the container (its max size)
			\return the capacity
		*/
		int getCapacity();
		
		/**
		 	\brief Get the fill state of the container
			\return True if the capacity is reached (False otherwise)
		*/
		bool isFilled();
		
		/**
			\brief Feed the container with a new value
			\param[in] x new value to store
			\return the state of the container
			
			\retval -1	The Ubend is not filled yet
			\retval 0	The Ubend is just filled
			\retval 1	Cruising case
		*/
		//int push(double x);
		UBENDSTATUS push(double x);
		
};

#endif // UBEND_H
