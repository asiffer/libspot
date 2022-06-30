#include "estimator.h"

using namespace std;

Estimator::Estimator(const Peaks &peaks) : peaks(peaks)
{
}

// Estimator::~Estimator() {}

/**
 * @brief best estimator among all implemented
 *
 * @param peaks
 * @param gamma
 * @param sigma
 */
Estimators gpd_fit(const Peaks &peaks, double *gamma, double *sigma)
{
    double logL0 = -numeric_limits<double>::infinity();
    double logL = 0.0;

    auto mom = MOMEstimator(peaks);
    auto grimshaw = GrimshawEstimator(peaks);
    Estimators estimators = {&mom, &grimshaw};

    // cout << "-----------" << endl;
    for (auto &e : estimators)
    {
        e->estimate();
        logL = peaks.log_likelihood(e->gamma, e->sigma);
        // cout << "gamma: " << e->gamma << " ";
        // cout << "sigma: " << e->sigma << " ";
        // cout << "LL: " << logL << endl;

        if (logL > logL0)
        {
            logL0 = logL;
            *gamma = e->gamma;
            *sigma = e->sigma;
        }
    }
    return estimators;
}

// ---------------------------------------------------------------------------
// MOM -----------------------------------------------------------------------
// ---------------------------------------------------------------------------

MOMEstimator::MOMEstimator(const Peaks &peaks) : Estimator(peaks) {}

void MOMEstimator::estimate()
{
    const double E = this->peaks.mean();
    const double V = this->peaks.var();
    const double R = E * E / V;

    this->gamma = 0.5 * (1 - R);
    this->sigma = 0.5 * E * (1 + R);

    // log-likelihood check (we must have x = gamma/sigma > -1/Ymax)
    // const double x = this->gamma / sigma;
    // const double x_min = -1. / this->peaks.max();
    // if (x < x_min)
    // {
    //     // try to modify sigma only

    // }
    // cout << "E=" << E << ",V=" << V << ",x=" << this->gamma / this->sigma << ",xmin=" << -1. / this->peaks.max() << endl;
}

// ---------------------------------------------------------------------------
// Grimshaw ------------------------------------------------------------------
// ---------------------------------------------------------------------------

GrimshawEstimator::GrimshawEstimator(const Peaks &peaks) : Estimator(peaks) {}

void GrimshawEstimator::estimate()
{
    // variable to retrieve peaksfit results
    double g;
    double s;
    double logL;

    // retrieve the candidates
    vector<double> candidates = this->roots();

    // logL0 = -oo
    double logL0 = -numeric_limits<double>::infinity();

    for (const auto x_star : candidates)
    {
        logL = this->simplified_log_likelihood(x_star, &g, &s);
        if (logL > logL0)
        {
            logL0 = logL;
            this->gamma = g;
            this->sigma = s;
        }
    }
}

vector<double> GrimshawEstimator::roots()
{
    // vector of roots
    vector<double> vec_roots;

    double min = this->peaks.min();
    double max = this->peaks.max();
    double mean = this->peaks.mean();
    double epsilon = std::min(1e-9, 0.5 / max);
    double r;
    double a, b;
    bool found = false;

    // 0 is always a root
    vec_roots.push_back(0.0);

    // the grimshaw function is bound to perform the brent root search
    using std::placeholders::_1;
    function<double(double)> f = std::bind(&GrimshawEstimator::w, this, _1);

    // left root
    a = -1 / max + epsilon;
    b = -epsilon;
    r = brent(&found, a, b, f);
    if (found)
    {
        vec_roots.push_back(r);
    }

    // right root
    a = epsilon;
    b = 2 * (mean - min) / (min * min);
    r = brent(&found, a, b, f);
    if (found)
    {
        vec_roots.push_back(r);
    }

    return vec_roots;
}

/**
 *  @brief Auxiliary function to compute the likelihood
 * @param x the antecedent
 *  @return w(x) = u(x)v(x) - 1
 */
double GrimshawEstimator::w(double x)
{
    size_t Nt_local = this->peaks.size();
    double u = 0.0;
    double v = 0.0;
    double s;

    for (const auto Yi : this->peaks)
    {
        s = 1 + x * Yi;
        u += 1 / s;
        v += log(s);
    }
    return (u / double(Nt_local)) * (1 + v / double(Nt_local)) - 1;
}

/**
 *  @brief Auxiliary function to compute the likelihood
 * @param x the antecedent
 * @return v(x)
 */
double GrimshawEstimator::v(double x)
{
    double v = 0.0;
    size_t Nt_local = this->peaks.size();
    for (const auto Yi : this->peaks)
    {
        v += log(1 + x * Yi);
    }
    return 1.0 + v / double(Nt_local);
}

double GrimshawEstimator::simplified_log_likelihood(double x_star, double *gamma, double *sigma)
{
    if (x_star == 0)
    {
        *gamma = 0.0;
        *sigma = this->peaks.mean();
    }
    else
    {
        *gamma = this->v(x_star) - 1;
        *sigma = *gamma / x_star;
    }

    return this->peaks.log_likelihood(*gamma, *sigma);
}