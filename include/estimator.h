/**
    @file estimator.h
    @brief Generic Estimator class implementation
    @author asr
*/

#include "peaks.h"

using namespace std;

#ifndef ESTIMATOR_H
#define ESTIMATOR_H

/**
 *	@class Estimator
 */
class Estimator
{
protected:
    const Peaks peaks;

public:
    double gamma;
    double sigma;

    /**
     * @brief Construct a new Estimator object
     *
     * @param peaks
     */
    Estimator(const Peaks &peaks);

    /**
     * @brief Destroy the Estimator object
     *
     */
    virtual ~Estimator(){};

    /**
     * @brief run the estimator (fit)
     *
     */
    virtual void estimate() = 0;
};

// list of estimators
using Estimators = vector<Estimator *>;

class MOMEstimator : public Estimator
{
public:
    /**
     * @brief Construct a new MOMEstimator object
     *
     * @param peaks
     */
    MOMEstimator(const Peaks &peaks);

    /**
     * @brief run the estimator (fit)
     *
     */
    void estimate();
};

class GrimshawEstimator : public Estimator
{
public:
    /**
     * @brief Construct a new Grimshaw Estimator object
     *
     * @param peaks
     */
    GrimshawEstimator(const Peaks &peaks);

    /**
     * @brief run the estimator (fit)
     *
     */
    void estimate();

private:
    /**
     * @brief Auxiliary function to compute the likelihood
     *
     * @param x the antecedent
     * @return double
     */
    double v(double x);

    /**
     * @brief Auxiliary function to compute the likelihood
     *
     * @param[in] x the antecedent
     * @return double
     */
    double w(double x);

    /**
     * @brief simplified log-likelihood function
     *
     * @param[in] x_star the antecedent
     * @param gamma GPD gamma parameter
     * @param sigma GPD sigma parameter
     * @return double
     */
    double simplified_log_likelihood(double x_star, double *gamma, double *sigma);

    /**
     * @brief Return the roots of the auxiliary function w
     *
     * @return vector<double>
     */
    vector<double> roots();
};

/**
 * @brief best estimator among all implemented
 *
 * @param peaks list of input peaks
 * @param[out] gamma GPD gamma parameter
 * @param[out] sigma GPD sigma parameter
 */
Estimators gpd_fit(const Peaks &peaks, double *gamma, double *sigma);

#endif // ESTIMATOR_H