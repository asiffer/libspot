#include "streamstats.h"


StreamStats::StreamStats(int size)
{
	this->s_star = 0.0;
	this->standard = StreamMoments(size);
	this->bootstrap = StreamVar(size);
	
	//TODO TEST
	//this->axes = StreamVar(size);
}



StreamStats::StreamStats(int size, vector<double> v, bool center) : StreamStats(size)
{
	double m = 0.0;
	if (center)
	{
		m = std::accumulate(v.begin(), v.end(), m);
		m = m/v.size();
	}
	for(auto x : v)
	{
		this->StreamStats::step(x-m);
	}
}


StreamStats StreamStats::merge(const StreamStats & S) const
{
	int s = std::min(this->standard.size(),S.standard.size());
	vector<double> data = this->standard;
	data.insert(data.end(),S.standard.begin(),S.standard.end());
	return StreamStats(s,data);
}


int StreamStats::size()
{
	return this->standard.size();
}


double StreamStats::B2()
{
	double Vred = this->bootstrap.var();
	
	//TODO TEST
	//double Vred = this->bootstrap.var() - 0.25*this->axes.var();
	
	double V = this->standard.var();
	return(4*Vred/V);
}



double StreamStats::H2()
{
	return this->bootstrap.hmean();
}


double StreamStats::get_s_star()
{
	return this->s_star;
}


int StreamStats::step(double x_n)
{
	this->standard.step(x_n);
	double m2 = this->standard.var();
	
	if (m2 > 0)
	{
		//double s_star_old = this->s_star;
		double m3 = this->standard.M3();
		double m1 = this->standard.mean();
		// double s_star_new = m1 + 0.5*m3/m2;
		this->s_star = m1 + 0.5*m3/m2;
		
		/*
		double r = (s_star_new-s_star_old)/s_star_old;
		double coeff = 10.;
		if ( std::abs(r)<coeff )
		{
			this->s_star = s_star_new;
		}
		else
		{
			this->s_star = s_star_old + coeff*(s_star_new-s_star_old)/std::abs(s_star_new-s_star_old);
		}*/
		
		
		this->bootstrap.step( std::abs(x_n - this->s_star) );
		
		//TODO TEST
		//this->axes.step(this->s_star);
	}
	
	return 0;
}


bool StreamStats::operator<(StreamStats& other)
{
	double m = this->standard.mean();
	double m_other = other.standard.mean();
	return (m<m_other);
}



vector<double> StreamStats::dump()
{
	return this->standard;
}


string StreamStats::stringStatus()
{
	stringstream ss;
	ss << '\t' << "---- Statistics ----" << endl;
	ss << '\t' << "Size" << '\t' << this->standard.size() << endl;
	ss << '\t' << "Mean" << '\t' << this->standard.mean() << endl;
	ss << '\t' << "Var" << '\t' << this->standard.var() << endl;
	ss << '\t' << "M3" << '\t' << this->standard.M3() << endl;
	ss << '\t' << "s*" << '\t' << this->s_star << endl;
	ss << '\t' << "Var|.|" << '\t' << this->bootstrap.var() << endl;
	ss << '\t' << "B2" << '\t' << this->B2() << endl;
	ss << '\t' << "H2" << '\t' << this->H2() << endl;
	return ss.str();
}




