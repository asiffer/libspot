#include "streammoments.h"

/* VERY LIGHT VERSION (only mean)*/
StreamMean::StreamMean(int size) : Ubend(size)
{
	this->m = 0.0;
	this->hm = 0.0;
}

StreamMean::StreamMean(int size, vector<double> v) : 
StreamMean(size)
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
	
	// harmonic mean
	this->hm += 1./x_n;
	
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
		
		// harmonic mean
		this->hm -= 1./x_1;
	}
	
	
	return status;
}


double StreamMean::mean()
{
	return(this->m);
}


double StreamMean::hmean()
{
	return(this->size()/this->hm);
}




StreamMean StreamMean::operator+(const StreamMean& other) const
{
	StreamMean sum(this->Ubend::merge(other));
	sum.m = std::accumulate(sum.begin(), sum.end(), 0.0) / sum.size();
	return sum;
}




/* LIGHT VERSION (only mean and variance)*/

StreamVar::StreamVar(int size) : StreamMean(size)
{
	this->m2 = 0.0;
}


StreamVar::StreamVar(int size, vector<double> v) : StreamVar(size)
{
	for(auto x : v)
	{
		this->StreamVar::step(x);
	}
}


double StreamVar::var()
{
	return this->m2/this->size();
}


int StreamVar::step(double x_n)
{
	int status = this->push(x_n);
	int n = this->size();
	
	double M = this->m;
	double delta, delta_n;
	
	// harmonic mean
	this->hm += 1./x_n;
	
	if (status <= 0)
	{
		delta = x_n - M;
		delta_n = delta/n;
	
		this->m = M + delta_n;
		this->m2 = this->m2 + (n-1.) * delta * delta_n;
		
	}
	else
	{
		double x_1 = this->last_erased_data;
		delta = x_n - x_1;
		delta_n = delta/n;
		
		this->m = this->m + delta_n;
		double u = x_n - this->m;
		this->m2 = this->m2 + delta * ( u + x_1 - M );
		
		// harmonic mean
		this->hm -= 1./x_1;
	}
	
	return status;
}





/* FULL VERSION (mean, variance and M3)*/

StreamMoments::StreamMoments(int size) : StreamVar(size)
{
	this->m3 = 0.0;
}


StreamMoments::StreamMoments(int size, vector<double> & v) : StreamMoments(size)
{
	for(auto & x: v)
	{
		this->StreamMoments::step(x);
	}
}




double StreamMoments::M3()
{
	return this->m3/this->size();
}


int StreamMoments::step(double x_n)
{
	int status = this->push(x_n);
	int n = this->size();
	
	double M = this->m;
	double delta, delta_n, delta_n2;
	
	// harmonic mean
	this->hm += 1./x_n;
	
	if (status <= 0)
	{
		delta = x_n - M;
		delta_n = delta/n;
		delta_n2 = delta_n * delta_n;
	
		this->m = M + delta_n;
		this->m3 = this->m3 + (n-1.)*(n-2.)*delta*delta_n2 - 3. * delta_n * (this->m2) ;
		this->m2 = this->m2 + (n-1.) * delta * delta_n;
	}
	else
	{
		double x_1 = this->last_erased_data;
		delta = x_n - x_1;
		delta_n = delta/n;
		delta_n2 = delta_n * delta_n;
		
		this->m = this->m + delta_n;
		double u = x_n - this->m;
		double v = x_1 - this->m;
		

		this->m3 = this->m3 - delta_n * ( 3. * this->m2 - n * delta_n2 ) + delta * ( pow(delta,2)+3*u*v ); 
		this->m2 = this->m2 + delta * ( u + x_1 - M );
		
		// harmonic mean
		this->hm -= 1./x_1;
	}
	
	
	return status;
}


