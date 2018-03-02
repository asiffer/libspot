#include "ubend.h"

using namespace std;



Ubend::Ubend(int N)
{
	this->cursor = 0;
	
	if (N>0)
	{
		this->capacity = N;
	}
	else
	{
		this->capacity = -1;
	}
}




Ubend Ubend::merge(const Ubend & other) const
{
	int N = std::min(this->capacity,other.capacity);
	Ubend merged(N);
	int a = 0;
	int b = 0;
	
	for (int i = 0; i<N; i++)
	{
		if (i%2 == 0)
		{
			merged.push( this->at( (this->cursor + a) % N ) );
			a++;
		}
		else
		{
			merged.push( other.at( (other.cursor + b) % N ) );
			b++;
		}
	}
	merged.cursor = 0;
	merged.last_erased_data = this->last_erased_data;
	return merged;
}



int Ubend::getCapacity()
{
	return this->capacity;
}


/*
int Ubend::push(double x)
{
	int s = this->size();

	if ( this->capacity == -1 ) // infinite capacity
	{
		this->push_back(x);
		return(-1);
	}
	else if ( s < this->capacity-1 )
	{
		this->push_back(x);
		return(-1);
	}
	else if ( s == this->capacity-1 )
	{
		this->push_back(x);
		return(0);
	}
	else
	{
		this->last_erased_data = this->operator[](this->cursor);
		this->operator[](this->cursor) = x;
		this->cursor = (this->cursor + 1) % this->capacity;
		return(1);
	}
	
}*/

UBENDSTATUS Ubend::push(double x)
{
	int s = this->size();

	if ( this->capacity == -1 ) // infinite capacity
	{
		this->push_back(x);
		return(UBENDSTATUS::INFINITE);
	}
	else if ( s < this->capacity-1 )
	{
		this->push_back(x);
		return(UBENDSTATUS::NOT_FILLED);
	}
	else if ( s == this->capacity-1 )
	{
		this->push_back(x);
		return(UBENDSTATUS::JUST_FILLED);
	}
	else
	{
		this->last_erased_data = this->operator[](this->cursor);
		this->operator[](this->cursor) = x;
		this->cursor = (this->cursor + 1) % this->capacity;
		return(UBENDSTATUS::CRUISING);
	}
	
}


int Ubend::getCursor()
{
	return this->cursor;
}


double Ubend::getLastErasedData()
{
	return this->last_erased_data;
}


bool Ubend::isFilled()
{
	return((int)this->size()==this->capacity);
}



