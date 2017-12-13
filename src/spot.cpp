/**
	@file spot.cpp
	@brief SPOT class implementation 
	@author Alban Siffer
	@date 19 May 2017
*/

#include "spot.h"


using namespace std;

/**
	@brief Default constructor
	@param[in] q Probability of abnormal events
	@param[in] n_init Size of initial batch to perform calibration
	@return Spot object
*/
Spot::Spot(double q, int n_init)
{
	this->q = q;
	this->bounded = true;
    this->max_excess = 200;
    this->alert = true;
    this->up = true;
    this->down = true;
	this->n_init = n_init;
	this->level = 0.98;
	
    this->n = 0;
    
    this->Nt_up = 0;
    this->Nt_down = 0;
    
    this->al_up = 0;
    this->al_down = 0;
    
    this->init_batch = vector<double>(this->n_init);
    
    if (up)
    {
    	this->upper_bound = GPDfit(max_excess);
    }

    if (down)
    {
		this->lower_bound = GPDfit(max_excess);
    }
}


/**
	@brief Constructor with risk parametrization (q) and initial batch
	@param[in] q Probability of abnormal events
	@param[in] init_data Initial batch to perform calibration
	@return Spot object
*/
Spot::Spot(double q, vector<double> init_data) : Spot::Spot(q,(int)init_data.size())
{
	this->init_batch = init_data;
	this->n = this->n_init;
	this->calibrate();
}


/**
	@brief Full parametrizable constructor
	@param[in] q Probability of abnormal events
	@param[in] n_init Number of initial observations to perform calibration
	@param[in] level Quantile level to discriminate the excesses from the normal data
	@param[in] up Compute upper threshold
	@param[in] down Compute lower threshold
	@param[in] alert Trigger alert
	@param[in] bounded Bounded mode
	@param[in] max_excess Maximum number of storable excesses (for bounded mode) 
	@return Spot object
*/
Spot::Spot(double q, int n_init, double level, 
		bool up, bool down, bool alert, 
		bool bounded, int max_excess)
{
	if (max_excess < 0)
	{
		max_excess = -1;
	}
	
    this->q = q;
    this->alert = alert;
    this->up = up;
    this->down = down;
    this->n = 0;
    this->al_up = 0;
    this->al_down = 0;
    this->Nt_up = 0;
    this->Nt_down = 0;
	this->level = level;
	
	if (n_init > 0)
	{
		this->n_init = n_init;
	}
	else
	{
		this->n_init = 1000;
	}

	this->init_batch = vector<double>(this->n_init);


    this->bounded = bounded;
    this->max_excess = max_excess;


    if (up)
    {
    	this->upper_bound = GPDfit(max_excess);
    }

    if (down)
    {
		this->lower_bound = GPDfit(max_excess);
    }

}




/**
	@brief Full parametrizable constructor with initial batch
	@param[in] q Probability of abnormal events
	@param[in] init_data Initial batch to perform calibration
	@param[in] level Quantile level to discriminate the excesses from the normal data
	@param[in] up Compute upper threshold
	@param[in] down Compute lower threshold
	@param[in] alert Trigger alert
	@param[in] bounded Bounded mode
	@param[in] max_excess Maximum number of storable excesses (for bounded mode) 
	@return Spot object
*/
Spot::Spot(double q, vector<double> init_data, double level, 
			bool up, bool down, bool alert, 
			bool bounded, int max_excess) : 
			Spot(q, (int)init_data.size(), level, up, down, alert, bounded, max_excess)
{
	this->init_batch = init_data;
	this->calibrate();
}



/**
	@brief create a Spot object with the same configuration
	@return Spot object
*/
Spot::Spot(SpotConfig sc)
{
	this->q = sc.q;
	this->bounded = sc.bounded;
	this->max_excess = sc.max_excess;
	this->alert = sc.alert;
	this->up = sc.up;
	this->down = sc.down;
	this->n_init = sc.n_init;
	this->level = sc.level;
	
	this->n = 0;
    
    this->Nt_up = 0;
    this->Nt_down = 0;
    
    this->al_up = 0;
    this->al_down = 0;
    
    if (up)
    {
    	this->upper_bound = GPDfit(max_excess);
    }

    if (down)
    {
		this->lower_bound = GPDfit(max_excess);
    }
    
    this->init_batch = vector<double>(this->n_init);
}


/**
	@brief Spot configuration comparison
	@param[in] spot Spot instance
	@return bool true: both instances have the same configuration
				false: instances have not the same configuraion
*/
bool Spot::operator==(const Spot &spot) const
{
	    
	bool valid = this->q == spot.q;
	valid &= this->bounded == spot.bounded;
	valid &= this->max_excess == spot.max_excess;
	valid &= this->alert == spot.alert;
	return valid;
}


/**
	@brief Merge 2 Spot instances
	@param[in] spot Spot instance
	@return Spot
	Beware of the order. Spot_1 + Spot_2 != Spot_2 + Spot_1
	This function takes the lower_bound of Spot_1 and the upper_bound of Spot_2
*/
Spot Spot::operator+(const Spot &spot) const
{
	    
	if ( this->operator==(spot) )
	{
		Spot spotsum(spot.config());
		spotsum.t_down = this->t_down;
		spotsum.z_down = this->z_down;
		spotsum.lower_bound = this->lower_bound;
		spotsum.Nt_down = this->Nt_down;
		spotsum.al_down = this->al_down;
		
		spotsum.t_up = spot.t_up;
		spotsum.z_up = spot.z_up;
		spotsum.upper_bound = spot.upper_bound;
		spotsum.Nt_up = spot.Nt_up;
		spotsum.al_up = spot.al_up;
		
		spotsum.n = this->n + spot.n;
		return spotsum;
		
	}
	else
	{
		cout << "Addition is not possible" << endl;
		return *this;
	}
}




/**
	@brief Spot iteration
	@param[in] v input data
	@return int 0: Normal data
				1/-1 : Up/Down alert
				2/-2 : Up/Down excess
				3: to initial batch
				4: calibration step
*/
int Spot::step(double v)
{

    (this->n)++;
    if (this->n < this->n_init)
    {
        this->init_batch[this->n-1] = v;
        return(3);
    }
    else if (this->n == this->n_init)
    {
    	this->init_batch[this->n-1] = v;
        this->calibrate();
        return(4);
    }
    else
    {
        if (this->up) // up check
        {

            if (this->alert && v>this->z_up) // check alert
            {
				this->n--;
				this->al_up++;
                return(1);
            }
            else if (v>this->t_up) // check update
            {
            	// increment Nt_up
				this->Nt_up++;
				
                // push value
                this->upper_bound.push( v - (this->t_up) );
                
                // fit
                this->fitup();
                
				return(2);
            }

        }

        if (this->down) // down check
        {
            if (this->alert && v<this->z_down) // check alert
            {
            	this->n--;
            	this->al_down++;
                return(-1);
            }
            else if (v<this->t_down) // check update
            {
            	// increment Nt_down
            	this->Nt_down++;

                // push value
                this->lower_bound.push(-(v-this->t_down));
                
                // update
                this->fitdown();
				return(-2);
            }

        }
        
        return(0);
    }
}


/**
	@brief GPD fit for the upper bound (update upper threshold)
*/
void Spot::fitup()
{
    this->z_up = this->threshold(this->upper_bound.fit(),this->t_up,this->Nt_up);
}

/**
	@brief GPD fit for the lower bound (update lower threshold)
*/
void Spot::fitdown()
{
    this->z_down = - this->threshold(this->lower_bound.fit(),-this->t_down,this->Nt_down);
}


/**
	@brief Compute threshold from the GPD fit
	@param[in] g GPDinfo (from the fit)
	@param[in] t excess quantile
	@param[in] Nt number of excesses (observations higher than t)
	@return threshold (z_q)
*/
double Spot::threshold(GPDinfo g, double t, int Nt)
{
    double z;
    double r = (this->q)*(this->n)/Nt;
    if (g.gamma == 0.0)
    {
        z = t - g.sigma * log(r);
    }
    else
    {
        z = t + (g.sigma/g.gamma) * ( pow(r,-g.gamma) - 1 );
    }
    return(z);
}



/**
	@brief Perform Spot initial calibration (after the first n_init observations)
*/
void Spot::calibrate()
{
	// we sort the initial batch to retrieve the quantiles
    sort(this->init_batch.begin(),this->init_batch.end());

    if (this->up) // if upper thresholding
    {
    	// get the quantile
        int rank_up = (int)(this->n_init * this->level);
        int Nt = (this->n_init)-rank_up;
        this->t_up = this->init_batch[rank_up];
        

        // Initialization of the upper excesses
        for(int i = rank_up+1; i < this->n_init; i++)
        {
            this->upper_bound.push( this->init_batch[i] - (this->t_up) );
        }
		
		this->Nt_up = this->upper_bound.size();
        GPDinfo info_up = this->upper_bound.fit();
        this->z_up = this->threshold(info_up,this->t_up,Nt);

    }

    if (this->down) // if lower thresholding
    {
    	// get the quantile
        int rank_down = (int)(this->n_init * (1 - this->level));
        this->t_down = this->init_batch[rank_down];
		
        // Initialization of the lower excesses
        for(int i = 0; i < rank_down; i++)
        {
            this->lower_bound.push(- ( this->init_batch[i]-this->t_down) );
        }
        
        this->Nt_down = this->lower_bound.size();
        GPDinfo info_down = this->lower_bound.fit();
        this->z_down = - this->threshold(info_down,- this->t_down,rank_down);
    }
   
}



// GETTER

/**
	@brief Get the current upper threshold
*/
double Spot::getUpperThreshold()
{
	return(this->z_up);
}

/**
	@brief Get the current lower threshold
*/
double Spot::getLowerThreshold()
{
	return(this->z_down);
}


/**
	@brief Get the current thresholds
*/
Bounds Spot::getThresholds()
{
	return(Bounds(this->z_down,this->z_up));
}

/**
	@brief Get the upper excess quantile
*/
double Spot::getUpper_t()
{
	return(this->t_up);
}

/**
	@brief Get the lower excess quantile
*/
double Spot::getLower_t()
{
	return(this->t_down);
}



/**
	@brief Get the internal state of the Spot instance
*/
SpotStatus Spot::status()
{
	SpotStatus status;
	status.n = this->n;
	status.Nt_up = this->Nt_up;
	status.Nt_down = this->Nt_down;
	status.ex_up = this->upper_bound.size();
	status.ex_down = this->lower_bound.size();
	status.al_up = this->al_up;
	status.al_down = this->al_down;
	status.t_up = this->t_up;
	status.t_down = this->t_down;
	status.z_up = this->z_up;
	status.z_down = this->z_down;
	return(status);
}


/**
	@brief Get the internal state of the Spot instance (display-ready)
*/
string Spot::stringStatus()
{
	return this->status().str();
}


/**
	@brief Get the configuration of the Spot instance (to create a new instance for example)
*/
SpotConfig Spot::config() const
{
	SpotConfig sc;
	sc.q = this->q;
	sc.bounded = this->bounded;
	sc.max_excess = this->max_excess;
	sc.alert = this->alert;
	sc.up = this->up;
	sc.down = this->down;
	sc.n_init = this->n_init;
	sc.level = this->level;
	return sc;
}



string SpotConfig::str()
{
	stringstream ss;
	string h = "---- Spot config ----";
	ss << '\t' << h << endl;
	ss.precision(4);
	ss << "\t" << "q" << "\t\t" << this->q << endl; 
	ss << "\t" << "n_init" << "\t\t" << this->n_init << endl;
	ss << "\t" << "level" << "\t\t" << this->level << endl; 
	ss << "\t" << "up" << "\t\t" << this->up << endl; 
	ss << "\t" << "down" << "\t\t" << this->down << endl; 
	ss << "\t" << "alert" << "\t\t" << this->alert << endl; 
	ss << "\t" << "bounded" << "\t\t" << this->bounded << endl; 
	ss << "\t" << "max_excess" << "\t" << this->max_excess << endl; 
	ss << endl;
	return ss.str();
}


/**
	@brief Format the status to print it
*/
string SpotStatus::str()
{
	stringstream ss;
	string h = "----- Spot status -----";
	ss << '\t' << h << endl;
	ss << '\t' << std::left << "n = " << setw(h.length()-4) << this->n << endl;
	ss << '\t' << setw(h.length()) << "" << endl;
	ss << "\t" << "info" << "\t" << "up" << "\t" << "down" << endl; 
	ss << '\t' << setfill('-') << setw(h.length()) << "" << endl;
	ss.precision(4);
	ss << "\t" << "Nt" << "\t" << this->Nt_up << "\t" << this->Nt_down << endl; 
	ss << "\t" << "ex" << "\t" << this->ex_up << "\t" << this->ex_down << endl; 
	ss << "\t" << "al" << "\t" << this->al_up << "\t" << this->al_down << endl; 
	ss << "\t" << "t" << "\t" << this->t_up << "\t" << this->t_down << endl; 
	ss << "\t" << "z" << "\t" << this->z_up << "\t" << this->z_down << endl; 
	ss << endl;
	return ss.str();
}
