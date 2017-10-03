#include <vector>

using namespace std;


#ifndef BOUNDS_H
#define BOUNDS_H

class Bounds {
	public:
	double down;
	double up;
	
	~Bounds();
	Bounds(double d = 0.0, double u = 0.0);
	double distanceTo(double x);
	bool contains(double x);
	bool operator<(Bounds const& other) const;
	bool operator>(Bounds const& other) const;
};

#endif // BOUNDS_H
