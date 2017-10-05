/* File : libstream.i */

%module libstream
%{
#include "ubend.h"
#include "streammoments.h"
#include "streamstats.h"
%}

/* Let's just grab the original header file here */
%include "std_string.i"
%include "std_vector.i"


namespace std {
   %template(DoubleVector) vector<double>;
}

%include "ubend.h"
%include "streammoments.h"
%include "streamstats.h"



