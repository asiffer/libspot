#include "dspot.h"


using namespace std;


Bounds operator+(Bounds B, double b);


DSpot::DSpot(int d) : Spot::Spot()
{
	this->depth = d;
	this->model = StreamMean(d);
}

DSpot::DSpot(int d, double q) : Spot::Spot(q)
{
	this->depth = d;
	this->model = StreamMean(d);
}

DSpot::DSpot(int d, double q, int n_init, double level, 
				bool up, bool down, bool alert, 
				bool bounded, int max_excess) : Spot::Spot(q, n_init, level, up, down, alert, bounded, max_excess)
{
	this->depth = d;
	this->model = StreamMean(d);
}


DSpot::DSpot(int d, double q, vector<double> init_data, bool shuffle) : Spot::Spot(q,(int)init_data.size()-d)
{
	if (shuffle)
	{
		std::random_device r;
    	std::default_random_engine rengine(r());
		std::shuffle(init_data.begin(),init_data.end(),rengine);
	}
	this->depth = d;
	this->model = StreamMean(d);
	for (auto & x: init_data)
	{
		this->step(x);
	}
}

DSpot::DSpot(int d, double q, vector<double> init_data, double level, 
    			bool up, bool down, bool alert, 
    			bool bounded, int max_excess, bool shuffle) : DSpot(d,q,(int)init_data.size()-d,level, up, down, alert, bounded, max_excess)
{
	if (shuffle)
	{
		std::random_device r;
    	std::default_random_engine rengine(r());
		std::shuffle(init_data.begin(),init_data.end(),rengine);
	}
	
	for (auto & x: init_data)
	{
		this->step(x);
	}
}


DSpot::DSpot(const DSpot & ds) : Spot(ds)
{
	this->depth = ds.depth;
}


bool DSpot::operator==(const DSpot &dspot) const
{
	bool valid = this->Spot::operator==((Spot)dspot);
	valid &= this->depth == dspot.depth;
	return valid;
}


DSpot DSpot::operator+(const DSpot & other) const
{
	if( this->operator==(other) )
	{
		double new_drift = 0.5*(this->drift + other.drift);
		
		DSpot dspotsum(this->depth, this->q, this->n_init,
						this->level, this->up, this->down, 
						this->alert, this->bounded, this->max_excess);
		
		dspotsum.model = this->model + other.model;
		
		dspotsum.t_down = this->t_down - new_drift + this->drift;
		dspotsum.z_down = this->z_down - new_drift + this->drift;

		dspotsum.t_up = other.t_up - new_drift + other.drift;
		dspotsum.z_up = other.z_up - new_drift + other.drift;

		dspotsum.drift = dspotsum.model.mean();		
		
		dspotsum.lower_bound = this->lower_bound;
		dspotsum.upper_bound = other.upper_bound;
		
		return dspotsum;
	}
	else
	{
		cout << "Addition is not possible" << endl;
		return *this;
	}
}


bool DSpot::operator<(const DSpot & other) const
{
	double sup = this->z_up + this->drift;
	double inf = other.z_down + other.drift;
	return (sup<inf);
}



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



double DSpot::computeDrift()
{
	this->drift = this->model.mean();
	return this->drift;
}


double DSpot::getDrift()
{
	return this->drift;
}
/*
double DSpot::btest()
{
	return this->stats.bimodalTest();
}*/


double DSpot::getUpperThreshold()
{
	return(this->z_up + this->drift);
}


double DSpot::getLowerThreshold()
{
	return(this->z_down + this->drift);
}


Bounds DSpot::getThresholds()
{
	return(Bounds(this->z_down + this->drift,this->z_up + this->drift));
}


SpotStatus DSpot::status()
{
	SpotStatus ss = this->Spot::status();
	ss.z_up += this->drift;
	ss.z_down += this->drift;
	ss.t_up += this->drift;
	ss.t_down += this->drift;
	return(ss);
}


string DSpot::stringStatus()
{
	stringstream ss;
	string h = "----- DSpot status ----";
	ss << '\t' << h << endl;
	ss << '\t' << std::left << "drift = " << setw(h.length()-8) << this->drift << endl;
	ss << '\t' << setw(h.length()) << "" << endl;
	ss << this->DSpot::status().str();
	return ss.str();
	/*
	string out = "n = " + to_string(s.n) << "  drift = " << s.drift << endl << endl;
	string s = this->Spot::stringStatus();
	cout << "\t\t -- DSpot status --" << endl;
	cout << "n = " << s.n << "  drift = " << s.drift << endl << endl;
	cout << "\t" << "info" << "\t" << "up" << "\t" << "down" << endl; 
	cout << "\t-----------------------" << endl;
	cout << "\t" << "Nt" << "\t" << s.Nt_up << "\t" << s.Nt_down << endl; 
	cout << "\t" << "ex" << "\t" << s.ex_up << "\t" << s.ex_down << endl; 
	cout << "\t" << "al" << "\t" << s.al_up << "\t" << s.al_down << endl; 
	cout << "\t" << "t" << "\t" << s.t_up << "\t" << s.t_down << endl; 
	cout << "\t" << "z" << "\t" << s.z_up << "\t" << s.z_down << endl; 
	cout << endl;*/
}

