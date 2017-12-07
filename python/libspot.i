/* File : spot.i */

%module libspot
%{
#include "ubend.h"
#include "streamstats.h"
#include "bounds.h"
#include "brent.h"
#include "gpdfit.h"
#include "spot.h"
#include "dspot.h"
%}

/* Let's just grab the original header file here */
%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"

namespace std {
   %template(DoubleVector) vector<double>;
   %template(BoundsVector) vector<Bounds>;
   %template(DSpotVector) vector<DSpot>;
   %template() std::pair<int,int>;
}

%include "ubend.h"
%include "streamstats.h"
%include "bounds.h"
%include "brent.h"
%include "gpdfit.h"
%include "spot.h"
%include "dspot.h"


