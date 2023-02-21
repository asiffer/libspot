/**
 * @file estimator.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-09-03
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "peaks.h"

#ifndef ESTIMATOR_H
#define ESTIMATOR_H

typedef double (*estimator)(struct Peaks const *, double *, double *);

/**
 * @brief
 *
 * @param tail
 * @param gamma
 * @param sigma
 * @return double
 */
double mom_estimator(struct Peaks const *peaks, double *gamma, double *sigma);

/**
 * @brief
 *
 * @param peaks
 * @param gamma
 * @param sigma
 * @return double
 */
double grimshaw_estimator(struct Peaks const *peaks, double *gamma,
                          double *sigma);
// /**
//  *   @class Estimator
//  */
// class Estimator
// {
// protected:
//     const Peaks peaks;

// public:
//     double gamma;
//     double sigma;

//     /**
//      * @brief Construct a new Estimator object
//      *
//      * @param peaks
//      */
//     Estimator(const Peaks &peaks);

//     /**
//      * @brief Destroy the Estimator object
//      *
//      */
//     // virtual ~Estimator(){};

//     /**
//      * @brief run the estimator (fit)
//      *
//      */
//     virtual void estimate() = 0;
// };

// // list of estimators
// using Estimators = vector<Estimator *>;

// class MOMEstimator : public Estimator
// {
// public:
//     /**
//      * @brief Construct a new MOMEstimator object
//      *
//      * @param peaks
//      */
//     MOMEstimator(const Peaks &peaks);

//     /**
//      * @brief run the estimator (fit)
//      *
//      */
//     void estimate();
// };

// class GrimshawEstimator : public Estimator
// {
// public:
//     /**
//      * @brief Construct a new Grimshaw Estimator object
//      *
//      * @param peaks
//      */
//     GrimshawEstimator(const Peaks &peaks);

//     /**
//      * @brief run the estimator (fit)
//      *
//      */
//     void estimate();

// private:
//     /**
//      * @brief Auxiliary function to compute the likelihood
//      *
//      * @param x the antecedent
//      * @return double
//      */
//     double v(double x);

//     /**
//      * @brief Auxiliary function to compute the likelihood
//      *
//      * @param[in] x the antecedent
//      * @return double
//      */
//     double w(double x);

//     /**
//      * @brief simplified log-likelihood function
//      *
//      * @param[in] x_star the antecedent
//      * @param gamma GPD gamma parameter
//      * @param sigma GPD sigma parameter
//      * @return double
//      */
//     double simplified_log_likelihood(double x_star, double *gamma, double
//     *sigma);

//     /**
//      * @brief Return the roots of the auxiliary function w
//      *
//      * @return vector<double>
//      */
//     vector<double> roots();
// };

// /**
//  * @brief best estimator among all implemented
//  *
//  * @param peaks list of input peaks
//  * @param[out] gamma GPD gamma parameter
//  * @param[out] sigma GPD sigma parameter
//  */
// Estimators gpd_fit(const Peaks &peaks, double *gamma, double *sigma);

#endif // ESTIMATOR_H
