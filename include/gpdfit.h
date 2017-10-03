#include <vector>
#include <iostream>
#include <functional>
#include <cmath>
#include <algorithm>

#include "ubend.h"
#include "brent.h"


using namespace std;


#ifndef GPDFIT_H
#define GPDFIT_H


struct GPDinfo {
    double gamma;
    double sigma;
    double llhood;
};


class GPDfit {
    private:
        // Attributes;
        Ubend excesses;

        // Methods (root search)
        double grimshaw_v(double x);
        GPDinfo log_likelihood(double x_star);
        vector<double> roots();

    public:

        // constructor/destructor
        GPDfit(int capacity = -1);

        // methods
        int size();
        double min();
        double max();
        double mean();

        void push(double v);
        //void append(double v);

        double grimshaw_w(double x);
        GPDinfo fit();

};

#endif // GPDFIT_H

