#include "dspot.h"


using namespace std;


Bounds operator+(Bounds B, double b);





StreamMean::StreamMean(int size) : Ubend(size)
{
	this->m = 0.0;
}

StreamMean::StreamMean(int size, vector<double> v) : StreamMean(size)
{
	for(auto x : v)
	{
		this->StreamMean::step(x);
	}
}


StreamMean::StreamMean(const Ubend & other) : Ubend(other)
{
	this->m = 0.0;
}


int StreamMean::step(double x_n)
{
	int status = this->push(x_n);
	int n = this->size();
	
	double M = this->m;
	double delta, delta_n;
	
	
	if (status <= 0)
	{
		delta = x_n - M;
		delta_n = delta/n;
		this->m = M + delta_n;
	}
	else
	{
		double x_1 = this->last_erased_data;
		delta = x_n - x_1;
		delta_n = delta/n;
		this->m = this->m + delta_n;
	}
	
	
	return status;
}


double StreamMean::mean()
{
	return(this->m);
}



StreamMean StreamMean::operator+(const StreamMean& other) const
{
	StreamMean sum(this->Ubend::merge(other));
	sum.m = std::accumulate(sum.begin(), sum.end(), 0.0) / sum.size();
	return sum;
}





DSpot::DSpot(int d, double q, int n_init) : Spot(q,n_init)
{
	this->depth = d;
	this->model = StreamMean(d);
}


DSpot::DSpot(int d, double q, vector<double> init_data) : DSpot(d,q,(int)(init_data.size()-d))
{
	for(auto & x : init_data)
	{
		this->DSpot::step(x);
	}
}


DSpot::DSpot(int d, double q, int n_init, double level, 
			bool up, bool down, bool alert, 
			bool bounded, int max_excess) : 
			Spot(q, n_init, level, up, down, alert, bounded, max_excess)
{
	this->model = StreamMean(d);
	this->depth = d;
}


DSpot::DSpot(int d, double q, vector<double> init_data, double level, 
			bool up, bool down, bool alert, 
			bool bounded, int max_excess) : 
			DSpot(d, q, (int)init_data.size(), level, up, down, alert, bounded, max_excess)
{
	for(auto & x : init_data)
	{
		this->step(x);
	}
}


/**
	@brief Test if the configurations are the same
	@return bool
*/
bool DSpot::operator==(const DSpot &dspot) const
{
	bool valid = this->Spot::operator==((Spot)dspot);
	valid &= this->depth == dspot.depth;
	return valid;
}

/**
	@brief Test if the upper bound of an instance is lower than the lower bound of the other
	@return bool
*/
bool DSpot::operator<(const DSpot & other) const
{
	double sup = this->z_up + this->drift;
	double inf = other.z_down + other.drift;
	return (sup<inf);
}


/**
	@brief DSpot iteration (like Spot iteration)
	@param[in] x input data
	@return int 0: Normal data
				1/-1 : Up/Down alert
				2/-2 : Up/Down excess
				3: to initial batch
				4: calibration step
*/
int DSpot::step(double x)
{
	int res = 0;
	int filled = -1;
	
	if (this->depth == 0) // SPOT case (no drift computation)
	{
		return this->Spot::step(x);
	}
	else if ( this->model.isFilled() ) // if the DSpot window is filled
	{

		// remove the current drift and send it to the spot instance
		res = this->Spot::step(x - this->drift);
		
		if (res*res != 1) // not an alarm
		{
			filled = this->model.step(x); // add data to the window
			this->drift = this->model.mean();
		}
	}
	else // if the DSpot window is not filled yet
	{
		filled = this->model.step(x);
	}
	
	if (filled == 0) // if the window has just been filled
	{
		this->drift = this->model.mean();
	}
	
	return(res);
}


/**
	@brief Compute, set and return the local drift
	@return double
*/
double DSpot::computeDrift()
{
	this->drift = this->model.mean();
	return this->drift;
}

/**
	@brief Compute, set and return the local drift
	@return double
*/
double DSpot::getDrift()
{
	return this->drift;
}

/**
	@brief Return the absolute upper threshold (with the drift)
	@return double
*/
double DSpot::getUpperThreshold()
{
	return(this->z_up + this->drift);
}

/**
	@brief Return the absolute lower threshold (with the drift)
	@return double
*/
double DSpot::getLowerThreshold()
{
	return(this->z_down + this->drift);
}

/**
	@brief Return the absolute thresholds (with the drift)
	@return Bounds object
*/
Bounds DSpot::getThresholds()
{
	return(Bounds(this->z_down + this->drift,this->z_up + this->drift));
}

/**
	@brief Get the internal state of the DSpot instance
*/
SpotStatus DSpot::status()
{
	SpotStatus ss = this->Spot::status();
	ss.z_up += this->drift;
	ss.z_down += this->drift;
	ss.t_up += this->drift;
	ss.t_down += this->drift;
	return(ss);
}

/**
	@brief Format the config to print it
*/
string DSpot::stringStatus()
{
	stringstream ss;
	string h = "----- DSpot status ----";
	ss << '\t' << h << endl;
	ss << '\t' << std::left << "drift = " << setw(h.length()-8) << this->drift << endl;
	ss << '\t' << setw(h.length()) << "" << endl;
	ss << this->DSpot::status().str();
	return ss.str();
}

