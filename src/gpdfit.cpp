#include "gpdfit.h"


using namespace std;



/**
	@brief Constructor
	@param[in] size_max maximum number of values to store
	@return GPDfit object
*/
GPDfit::GPDfit(int capacity) : Ubend(capacity)
{
	this->gamma = 0.0;
	this->sigma = 0.0;
	this->llhood = 0.0;
}



/**
	@brief Return the smallest stored excess
*/
double GPDfit::min()
{	
	return *std::min_element(this->begin(),this->end());
}


/**
	@brief Return the largest stored excess
*/
double GPDfit::max()
{
	return *std::max_element(this->begin(),this->end());
}


/**
	@brief Return the mean of the stored excesses
*/
double GPDfit::mean()
{	
	double sum = std::accumulate(this->begin(),this->end(),0.0);
	return sum/this->size();
}



// fit


/**
	@brief Auxiliary function to compute the likelihood
	@param[in] x the antecedent
	@return v(x) 
*/
double GPDfit::grimshaw_v(double x)
{
	double v = 0.0;
	for(auto it = this->begin(); it != this->end(); ++it)
	{
		v += log( 1 + x * (*it) );
	}
	return( 1+v/this->size() );
}

/**
	@brief Auxiliary function to compute the likelihood
	@param[in] x the antecedent
	@return w(x) = u(x)v(x) - 1 
*/
double GPDfit::grimshaw_w(double x)
{
	double Nt_local = this->size();
	double u = 0.0;
	double v = 0.0;
	double s;
	
	for(auto it = this->begin(); it != this->end(); ++it)
	{
		s = 1 + x * (*it);
		u += 1/s;
		v += log(s);
	}
	return( (u/Nt_local) * ( 1 + v/Nt_local ) - 1);
}


/**
	@brief simplified log-likelihood function
	@param[in] x_star the antecedent
	@return GPDinfo object gathering gamma, sigma and the likelihood 
*/
/*
GPDinfo GPDfit::log_likelihood(double x_star)
{
	GPDinfo info_x;
	double Nt_local = this->size();
	info_x.gamma = this->grimshaw_v(x_star)-1;

	if (info_x.gamma == 0)
	{
		info_x.sigma = this->mean();
		info_x.llhood = Nt_local * ( 1 + log(info_x.sigma) );
	}
	else
	{
		info_x.sigma = info_x.gamma/x_star;
		info_x.llhood = -Nt_local*log(info_x.sigma)-Nt_local*(1+info_x.gamma);
	}

	return( info_x );
}

*/
double GPDfit::log_likelihood(double x_star, double * g, double * s)
{
	double Nt_local = this->size();
	*g = this->grimshaw_v(x_star)-1;

	if (*g == 0.0)
	{
		*s = this->mean();
		return ( Nt_local * ( 1 + std::log(*s) ) );
	}
	else
	{
		*s = *g/x_star;
		return ( -Nt_local*std::log(*s)-Nt_local*(1 + *g) );
	}
}


/**
	@brief Return the roots of the auxiliary function w
*/
vector<double> GPDfit::roots()
{
	// vector of roots
	vector<double> vec_roots;
	
	
	double min = this->min();
	double max = this->max();
	double mean = this->mean();
	double epsilon = std::min(1e-9,0.5/max);
	double r;
	double a,b;
	bool found = false;

	// 0 is always a root
	vec_roots.push_back(0.0);

	// the grimshaw function is bound to perform the brent root search
	using std::placeholders::_1;
	function<double(double)> f = std::bind( &GPDfit::grimshaw_w, this, _1 );

	// left root
	a = -1/max + epsilon;
	b = -epsilon;
	r = brent(&found,a,b,f);
	if (found)
	{
		vec_roots.push_back(r);
	}

	// right root
	a = 2*(mean-min)/(mean*min);
	b = 2*(mean-min)/(min*min);
	r = brent(&found,a,b,f);
	if (found)
	{
		vec_roots.push_back(r);
	}

	return(vec_roots);
}

/**
	@brief Perform a GPD fit of the stored excesses
	@return GPDinfo object gathering gamma, sigma and the likelihood 
*/
//GPDinfo GPDfit::fit()
void GPDfit::fit()
{
	// retrieve the candidates
	vector<double> candidates = this->roots();
	vector<double>::iterator it = candidates.begin();

	// zero is always candidate
	double x_star = *it;
	it++;

	double g;
	double s;
	// we use the likelihood of zero as a base
	double l = this->log_likelihood(x_star, &g, &s);
	this->gamma = g;
	this->sigma = s;
	
	double llh;

	// we look for better likelihood across the roots
	for(; it != candidates.end(); ++it)
	{
		llh = this->log_likelihood(x_star, &g, &s);
		if (llh > l)
		{
			l = llh;
			this->gamma = g;
			this->sigma = s;
			this->llhood = llh;
		}
	}
}

/*

void GPDfit::fit()
{
	// retrieve the candidates
	vector<double> candidates = this->roots();
	vector<double>::iterator it = candidates.begin();

	// zero is always candidate
	double x_star = *it;
	it++;

	// we use the likelihood of zero as a base
	GPDinfo a = this->log_likelihood(x_star);
	GPDinfo b;

	// we look for better likelihood across the roots
	for(; it != candidates.end(); ++it)
	{
		b = this->log_likelihood(*it);
		if (b.llhood > a.llhood)
		{
			a = b;
		}
	}

	// update the internal state
	this->info = a;
	
	//return(a);
}*/


double GPDfit::quantile(double q, double t, int n, int Nt)
{
	double r = (q * n) / Nt;
	if (this->gamma == 0.0)
	{
		return ( t - this->sigma * std::log(r) );
	}
	else
	{
		return ( t + (this->sigma/this->gamma) * ( pow(r, - this->gamma) - 1 ) );
	}
}



double GPDfit::probability(double z, double t, int n, int Nt)
{
	double r = (z - t)*(this->gamma/this->sigma);
	double s = 1.*Nt/n;
	if (this->gamma == 0.0)
	{
		return ( s * std::exp( - (z - t)/this->sigma ) );
	}
	else
	{
		return ( s * pow(1+r, - 1.0/this->gamma) );
	}
}


/*
GPDfit::GPDfit(int capacity)
{
	this->excesses = Ubend(capacity);
}


int GPDfit::size()
{
	return(this->excesses.size());
}


double GPDfit::min()
{	
	double mini = this->excesses[0];
	for(auto & d : this->excesses)
	{
		mini = std::min(mini,d);
	}
	return(mini);
}


double GPDfit::max()
{
	double maxi = this->excesses[0];
	for(auto & d : this->excesses)
	{
		maxi = std::max(maxi,d);
	}
	return(maxi);
}


double GPDfit::mean()
{	
	double sum = 0;
	
	for(auto & d : this->excesses)
	{
		sum += d;
	}
	return(sum/this->size());
}


void GPDfit::push(double v)
{
	this->excesses.push(v);
}	



// fit



double GPDfit::grimshaw_v(double x)
{

	double v = 0;
	for(auto & d : this->excesses)
	{
		v += log( 1 + x * d );
	}
	return( 1+v/this->size() );
}


double GPDfit::grimshaw_w(double x)
{
	double Nt_local = this->size();
	double u = 0;
	double v = 0;
	double s;
	
	for(auto & d : this->excesses)
	{
		s = 1 + x * d;
		u += 1/s;
		v += log(s);
	}
	return( (u/Nt_local) * ( 1 + v/Nt_local ) - 1);
}


GPDinfo GPDfit::log_likelihood(double x_star)
{
	GPDinfo info;
	double Nt_local = this->size();
	info.gamma = this->grimshaw_v(x_star)-1;

	if (info.gamma == 0)
	{
		info.sigma = this->mean();
		info.llhood = Nt_local * ( 1 + log(info.sigma) );
	}
	else
	{
		info.sigma = info.gamma/x_star;
		info.llhood = -Nt_local*log(info.sigma)-Nt_local*(1+info.gamma);
	}
	return info;
}


vector<double> GPDfit::roots()
{
	// vector of roots
	vector<double> vec_roots;
	
	
	double min = this->min();
	double max = this->max();
	double mean = this->mean();
	double epsilon = std::min(1e-9,0.5/max);
	double r;
	double a,b;
	bool found = false;

	// 0 is always a root
	vec_roots.push_back(0.0);

	// the grimshaw function is bound to perform the brent root search
	using std::placeholders::_1;
	function<double(double)> f = std::bind( &GPDfit::grimshaw_w, this, _1 );

	// left root
	a = -1/max + epsilon;
	b = -epsilon;
	r = brent(&found,a,b,f);
	if (found)
	{
		vec_roots.push_back(r);
	}

	// right root
	a = 2*(mean-min)/(mean*min);
	b = 2*(mean-min)/(min*min);
	r = brent(&found,a,b,f);
	if (found)
	{
		vec_roots.push_back(r);
	}

	return(vec_roots);
}


GPDinfo GPDfit::fit()
{
	// retrieve the candidates
	vector<double> candidates = this->roots();
	vector<double>::iterator it = candidates.begin();

	// zero is always candidate
	double x_star = *it;
	it++;

	// we use the likelihood of zero as a base
	GPDinfo a = this->log_likelihood(x_star);
	GPDinfo b;

	// we look for better likelihood across the roots
	for(; it != candidates.end(); ++it)
	{
		b = this->log_likelihood(*it);
		if (b.llhood > a.llhood)
		{
			a = b;
		}
	}

	return(a);
}
*/

