#include "bounds.h"


Bounds::Bounds(double d, double u)
{
	this->up = u;
	this->down = d;
}


bool Bounds::operator<(Bounds const& other) const
{
	return(this->up < other.down);
}

bool Bounds::operator>(Bounds const& other) const
{
	return(this->down > other.up);
}


double Bounds::distanceTo(double x)
{
	if (x<this->down)
	{
		return (this->down - x);
	}
	else if (x>this->up)
	{
		return (x - this->up);
	}
	else
	{
		return 0.0;
	}
}

bool Bounds::contains(double x)
{
	return( (x>=this->down) && (x<=this->up) );
}

Bounds operator+(Bounds B, double b)
{
	B.up += b;
	B.down += b;
	return B;
}

Bounds operator-(Bounds B, double b)
{
	B.up -= b;
	B.down -= b;
	return B;
}
