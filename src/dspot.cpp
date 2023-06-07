#include "dspot.h"

using namespace std;

Bounds operator+(Bounds B, double b);

StreamMean::StreamMean(int size) : Ubend(size)
{
	this->m = 0.0;
}

StreamMean::StreamMean(int size, vector<double> v) : StreamMean(size)
{
	for (auto x : v)
	{
		this->StreamMean::step(x);
	}
}

StreamMean::StreamMean(const Ubend &other) : Ubend(other)
{
	this->m = 0.0;
}

UBENDSTATUS StreamMean::step(double x_n)
{
	UBENDSTATUS status = this->push(x_n);
	int n = this->size();

	double M = this->m;
	double delta, delta_n;

	if (status == UBENDSTATUS::CRUISING)
	{
		double x_1 = this->last_erased_data;
		delta = x_n - x_1;
		delta_n = delta / n;
		this->m = this->m + delta_n;
	}
	else
	{
		delta = x_n - M;
		delta_n = delta / n;
		this->m = M + delta_n;
	}

	/*
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
	}*/

	return status;
}

double StreamMean::mean()
{
	return (this->m);
}

StreamMean StreamMean::operator+(const StreamMean &other) const
{
	StreamMean sum(this->Ubend::merge(other));
	sum.m = std::accumulate(sum.begin(), sum.end(), 0.0) / sum.size();
	return sum;
}

DSpot::DSpot(int d, double q, int n_init) : Spot(q, n_init)
{
	this->depth = d;
	this->model = StreamMean(d);
}

DSpot::DSpot(int d, double q, vector<double> init_data) : DSpot(d, q, (int)(init_data.size() - d))
{
	for (auto &x : init_data)
	{
		this->DSpot::step(x);
	}
}

DSpot::DSpot(int d, double q, int n_init, double level,
			 bool up, bool down, bool alert,
			 bool bounded, int max_excess) : Spot(q, n_init, level, up, down, alert, bounded, max_excess)
{
	this->model = StreamMean(d);
	this->depth = d;
}

DSpot::DSpot(int d, double q, vector<double> init_data, double level,
			 bool up, bool down, bool alert,
			 bool bounded, int max_excess) : DSpot(d, q, (int)init_data.size(), level, up, down, alert, bounded, max_excess)
{
	for (auto &x : init_data)
	{
		this->step(x);
	}
}

// experimental
DSpot::DSpot(int d, double q, int n_init, double level,
			 int up, int down, int alert, int bounded, int max_excess) : DSpot(d, q, n_init, level, up == 1, down == 1, alert == 1, bounded == 1, max_excess)
{
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
bool DSpot::operator<(const DSpot &other) const
{
	double sup = this->z_up + this->drift;
	double inf = other.z_down + other.drift;
	return (sup < inf);
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
/* deprecated (old version)
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
*/

SPOTEVENT DSpot::step(double x)
{
	SPOTEVENT res = OTHER;
	int filled = -1;

	if (this->depth == 0) // SPOT case (no drift computation)
	{
		return this->Spot::step(x);
	}
	else if (this->model.isFilled()) // if the DSpot window is filled
	{

		// remove the current drift and send it to the spot instance
		res = this->Spot::step(x - this->drift);

		if ((res != SPOTEVENT::ALERT_UP) && (res != SPOTEVENT::ALERT_DOWN)) // not an alarm
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

	return (res);
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
	return (this->z_up + this->drift);
}

/**
	@brief Return the absolute lower threshold (with the drift)
	@return double
*/
double DSpot::getLowerThreshold()
{
	return (this->z_down + this->drift);
}

/**
	@brief Return the absolute thresholds (with the drift)
	@return Bounds object
*/
Bounds DSpot::getThresholds()
{
	return (Bounds(this->z_down + this->drift, this->z_up + this->drift));
}

/**
	@brief Return the current state of the DSpot instance through a single line string
*/
string DSpot::log(int log_level)
{
	stringstream ss;
	ss.precision(4);
	ss << std::left;
	const int w = 10;

	if (log_level >= 0)
	{
		ss << setw(w) << this->drift;
		ss << setw(w) << this->z_down;
		ss << setw(w) << this->z_up;
	}

	if (log_level >= 1)
	{
		ss << setw(w) << this->n;
		ss << setw(w) << this->al_down;
		ss << setw(w) << this->al_up;
	}

	if (log_level >= 2)
	{
		ss << setw(w) << this->Nt_down;
		ss << setw(w) << this->Nt_up;
	}
	ss << endl;
	return ss.str();
}

/**
	@brief Get the initial config of the DSpot instance
*/
DSpotConfig DSpot::config() const
{
	DSpotConfig dsc;
	dsc.q = this->q;
	dsc.bounded = this->bounded;
	dsc.max_excess = this->max_excess;
	dsc.alert = this->alert;
	dsc.up = this->up;
	dsc.down = this->down;
	dsc.n_init = this->n_init;
	dsc.level = this->level;
	dsc.depth = this->depth;
	return (dsc);
}

string DSpotConfig::str()
{
	stringstream ss;
	string h = "---- DSpot config ----";
	ss << h << endl;
	ss.precision(4);
	ss << "depth"
	   << "\t\t" << this->depth << endl;
	ss << "q"
	   << "\t\t" << this->q << endl;
	ss << "n_init"
	   << "\t\t" << this->n_init << endl;
	ss << "level"
	   << "\t\t" << this->level << endl;
	ss << "up"
	   << "\t\t" << this->up << endl;
	ss << "down"
	   << "\t\t" << this->down << endl;
	ss << "alert"
	   << "\t\t" << this->alert << endl;
	ss << "bounded"
	   << "\t\t" << this->bounded << endl;
	ss << "max_excess"
	   << "\t" << this->max_excess << endl;
	ss << endl;
	return ss.str();
}

/**
	@brief Get the internal state of the DSpot instance
*/
DSpotStatus DSpot::status()
{
	DSpotStatus status;
	status.n = this->n;
	status.ex_up = this->upper_bound.size();
	status.ex_down = this->lower_bound.size();
	status.Nt_up = this->Nt_up;
	status.Nt_down = this->Nt_down;
	status.al_up = this->al_up;
	status.al_down = this->al_down;
	status.t_up = this->t_up + this->drift;
	status.t_down = this->t_down + this->drift;
	status.z_up = this->z_up + this->drift;
	status.z_down = this->z_down + this->drift;
	status.drift = this->drift;
	return (status);
}

/**
	@brief Format the status to print it
*/
string DSpotStatus::str()
{
	stringstream ss;
	string h = "----- DSpot status -----";
	ss << h << endl;
	ss << std::left << "n = " << setw(h.length() - 4) << this->n << endl;
	ss << std::left << "drift = " << setw(h.length() - 4) << this->drift << endl;
	ss << setw(h.length()) << "" << endl;
	ss << "info"
	   << "\t"
	   << "up"
	   << "\t"
	   << "down" << endl;
	ss << setfill('-') << setw(h.length()) << "" << endl;
	ss.precision(4);
	ss << "Nt"
	   << "\t" << this->Nt_up << "\t" << this->Nt_down << endl;
	ss << "ex"
	   << "\t" << this->ex_up << "\t" << this->ex_down << endl;
	ss << "al"
	   << "\t" << this->al_up << "\t" << this->al_down << endl;
	ss << "t"
	   << "\t" << this->t_up << "\t" << this->t_down << endl;
	ss << "z"
	   << "\t" << this->z_up << "\t" << this->z_down << endl;
	ss << endl;
	return ss.str();
}

/**
	@brief Format the config to print it
*/
string DSpot::stringStatus()
{
	/*stringstream ss;
	string h = "----- DSpot status ----";
	ss << h << endl;
	ss << std::left << "drift = " << setw(h.length()-8) << this->drift << endl;
	ss << setw(h.length()) << "" << endl;
	ss << this->DSpot::status().str();
	return ss.str();*/
	return this->DSpot::status().str();
}

/**
	\brief Get the upper excess quantile
	\details Overload Spot method (return the real absolute value with the drift)
*/
double DSpot::getUpper_t()
{
	return (this->t_up + this->drift);
}

/**
	\brief Get the lower excess quantile
	\details Overload Spot method (return the real absolute value with the drift)
*/
double DSpot::getLower_t()
{
	return (this->t_up + this->drift);
}

/**
	\brief Give the probability to observe things higher than a value
	\details Overload Spot method (return the real absolute value with the drift)
	\param[in] z input value
	\return probability 1-F(z)
*/
double DSpot::up_probability(double z)
{
	return (this->Spot::up_probability(z) + this->drift);
}

/**
	\brief Give the probability to observe things lower than a value
	\details Overload Spot method (return the real absolute value with the drift)
	\param[in] z input value
	\return probability F(z)
*/
double DSpot::down_probability(double z)
{
	return (this->Spot::down_probability(z) + this->drift);
}
