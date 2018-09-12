#include "spot.h"
#include <random>
#include <iostream>
#include <iomanip>
#include <chrono>


/* COLORS FOR FANCY PRINT */
#define END  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"

#define OK "[" GRN "OK" END "]"
#define WARNING "[" YEL "WARNING" END "]"
#define FAIL "[" RED "FAIL" END "]"
/* END OF COLORS FOR FANCY PRINT */


using namespace std;
using T = std::chrono::steady_clock::time_point;
using usec = std::chrono::microseconds;
using sec = std::chrono::seconds;
const auto& tick = std::chrono::steady_clock::now;


int duration(T start, T end)
{
	return std::chrono::duration_cast<usec>(end - start).count();
}

int operator-(const T& end, const T& start)
{
	return(duration(start,end));
}

double alea()
{
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_real_distribution<double> u(0,1);
	return u(gen);
}


vector<double> uniform(double a, double b, int N)
{
	vector<double> v(N);
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_real_distribution<double> u(a,b);

	for (int i = 0; i < N; i++)
	{
		v[i] = u(gen);
	}

	return(v); 
}



vector<double> gaussian(double mu, double sigma, int N)
{
	vector<double> v(N);
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::normal_distribution<double> d(mu,sigma);

	for (int i = 0; i < N; i++)
	{
		v[i] = d(gen);
	}

	return(v);   
}


void check_thresholds(Spot & S, double z_truth)
{
	double r = 0.0;
	
	r = std::abs(S.getUpperThreshold() - z_truth);
	cout << setw(20) << std::left << "Upper threshold"; 
	if ( r < 0.1)
	{
		cout << OK << endl;
	}
	else if ( r < 0.6)
	{
		cout << WARNING << " (maybe anomalies are flagged or you are not lucky, try again to verify)" << endl;
	}
	else
	{
		cout << FAIL << " (maybe anomalies are flagged or you are not lucky, try again to verify)" << endl;
	}
	
	r = std::abs(S.getLowerThreshold() + z_truth);
	cout << setw(20) << std::left << "Lower threshold";
	if ( r < 0.1)
	{
		cout << OK << endl;
	}
	else if ( r < 0.6)
	{
		cout << WARNING << " (maybe anomalies are flagged or you are not lucky, try again to verify)" << endl;
	}
	else
	{
		cout << FAIL << " (maybe anomalies are flagged or you are not lucky, try again to verify)" << endl;
	}
}

vector<double> linspace(double min, double max, int n_points)
{
	vector<double> v(n_points);
	double step = (max - min)/(n_points - 1);
	for (int i = 0; i<n_points; i++)
	{
		v[i] = min + i*step;
	}
	return v;
}

void check_up_proba(Spot & S)
{
	double q = S.config().q;
	double z_q = S.getUpperThreshold();
	vector<double> v = linspace(z_q, 5., 20);
	
	cout << setw(30) << std::left << "Upper threshold meaning"; 
	double rel_err = abs(q - S.up_probability(z_q))/q;
	if ( rel_err < 0.01 )
	{
		cout << OK << endl;
	}
	else if ( rel_err < 0.02 )
	{
		cout << WARNING << endl;
	}
	else {
		cout << FAIL << endl;
	}

	cout << setw(30) << std::left << "Probability computation";
	double ref = S.up_probability(z_q);
	double p = 0.;
	for (auto & z: v)
	{
		p = S.up_probability(z);
		if (p > ref)
		{
			cout << FAIL << endl;
			return;
		}
		else
		{
			ref = p;
		}
		
		// cout << "P(X>" << setprecision(3) << z << ") = " << setprecision(5) << S.up_probability(z) << endl;
		// cout << "P(X>" << z << ") = " << S.up_probability(z) << endl;
	}
	cout << OK << endl;
}



void check_down_proba(Spot & S)
{
	double q = S.config().q;
	double z_q = S.getLowerThreshold();
	vector<double> v = linspace(z_q, -5., 20);
	
	cout << setw(30) << std::left << "Lower threshold meaning"; 
	double rel_err = abs(q - S.down_probability(z_q))/q;
	if ( rel_err < 0.01 )
	{
		cout << OK << endl;
	}
	else if ( rel_err < 0.02 )
	{
		cout << WARNING << endl;
	}
	else {
		cout << FAIL << endl;
	}

	cout << setw(30) << std::left << "Probability computation";
	double ref = S.down_probability(z_q);
	double p = 0.;
	for (auto & z: v)
	{
		p = S.down_probability(z);
		//cout << "P(X<" << setprecision(3) << z << ") = " << setprecision(5) << p << endl;
		if (p > ref)
		{
			cout << FAIL << endl;
			return;
		}
		else
		{
			ref = p;
		}
	}
	cout << OK << endl;
}




void check_flagging(Spot & S)
{
	SpotStatus status = S.status();
	int al_up = status.al_up;
	int al_down = status.al_down;
	
	cout << setw(30) << std::left << "Flagging up anomalies"; 
	if ( ( al_up >= 18 ) && ( al_up <= 66 ) ) // these bounds are specific to N = 20000
	{
		cout << OK << endl;
	}
	else if ( ( al_up < 13 ) || ( al_up > 85 ) ) // these bounds are specific to N = 20000
	{
		cout << FAIL << " (this is abnormal or you are not lucky, try again to verify)" << endl;
	}
	else
	{
		cout << WARNING << " (maybe you are not lucky, try again to verify)" << endl;
	}
	
	cout << setw(30) << std::left << "Flagging down anomalies"; 
	if ( ( al_down >= 18 ) && ( al_down <= 66 ) ) // these bounds are specific to N = 20000
	{
		cout << OK << endl;
	}
	else if ( ( al_down < 13 ) || ( al_down > 85 ) ) // these bounds are specific to N = 20000
	{
		cout << FAIL << " (this is abnormal or you are not lucky, try again to verify)" << endl;
	}
	else
	{
		cout << WARNING << " (maybe you are not lucky, try again to verify)" << endl;
	}
	
}


int main(int argc, const char * argv[])
{
	// the tests depend on these parameters
	double mu = 0.0;
	double sigma = 1.0;
	double z_truth = 3.09;
	int N = 20000;
	vector<double> data = gaussian(mu,sigma,N);
	// ------------------------------------
	
	
	cout << endl << "\t1. Quantile computation" << endl;
	Spot S(1e-3,2000,0.99,true,true,false,true,200);
	
	for(auto x : data)
	{
		S.step(x);
	}
	cout << endl << S.stringStatus() << endl;
	check_thresholds(S, z_truth);
	
	cout << endl << "\t2. Anomaly flagging" << endl;
	S = Spot(1e-3,2000);
	for(auto x : data)
	{
		S.step(x);
	}
	cout << endl << S.stringStatus() << endl;
	check_flagging(S);

	cout << endl << "\t3. Probability computation\n" << endl;
	check_up_proba(S);
	check_down_proba(S);

	return 0;
}
