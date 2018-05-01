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
	return 0;
}
