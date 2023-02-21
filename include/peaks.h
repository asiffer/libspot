/**
 * @file peaks.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "brent.h"
#include "ubend.h"
#include "xmath.h"

#ifndef PEAKS_H
#define PEAKS_H

/**
 * @brief
 *
 */
struct Peaks {
    double e;               // sum of the elements
    double e2;              // sum of the square of the elements
    double min;             // min of the elements
    double max;             // max of the elements
    struct Ubend container; // underlying data
};

/**
 * @brief
 *
 * @param peaks
 * @param size
 * @return int
 */
int peaks_init(struct Peaks *peaks, unsigned long size);

/**
 * @brief
 *
 * @param peaks
 */
void peaks_free(struct Peaks *peaks);

/**
 * @brief
 *
 * @param peaks
 */
void peaks_push(struct Peaks *peaks, double);

/**
 * @brief
 *
 * @param peaks
 * @return double
 */
double peaks_mean(struct Peaks const *peaks);

/**
 * @brief
 *
 * @param peaks
 * @return double
 */
double peaks_var(struct Peaks const *peaks);

/**
 * @brief Return the number of peaks
 *
 * @param peaks input structure
 * @return unsigned long number of peaks
 */
unsigned long peaks_size(struct Peaks const *peaks);

/**
 * @brief Compute the GPD log-likelihood function
 *
 * @param[in] peaks peaks structure
 * @return double
 */
double log_likelihood(struct Peaks const *peaks, double gamma, double sigma);

/**
 * @brief Compute the GPD log-likelihood function
 *
 * @param[in] peaks peaks structure
 * @return double
 */
// double log_likelihood_self(struct Peaks const *peaks);

// class Peaks : public Ubend
// {
// private:
//  double gamma;
//  double sigma;

// public:
//  /**
//   * \brief Construct a new Peaks object
//   *
//   * \param capacity maximum number of values to store
//   */
//  Peaks(int capacity = -1);

//  /**
//   *  \brief Get the minimum value of stored data
//   */
//  double min() const;

//  /**
//   *  \brief Get the maximum value of stored data
//   */
//  double max() const;

//  /**
//   *  \brief Get the sum of stored data
//   */
//  double sum() const;

//  /**
//   * \brief Get the mean of stored data
//   */
//  double mean() const;

//  /**
//   *  \brief Return the variance of the stored excesses
//   */
//  double var() const;

//  /* ====================== Stuff specific to GPD  ====================== */

//  /**
//   * \brief set gamma et sigma parameters
//   */
//  void set_gpd_parameters(double gamma, double sigma);

//  /**
//   *  \brief compute the GPD log-likelihood function
//   *  \param[in] g pointer to gamma parameter
//   *  \param[in] s pointer to sigma parameter
//   *  \return log-likelihood
//   */
//  double log_likelihood(double gamma, double sigma) const;

//  /**
//   *  \brief Given some extra parameters, compute the probability 1-F(z)
//   *  \param z Desired quantile
//   *  \param t Intermediate threshold
//   *  \param n Total number of observations
//   *  \param Nt Total number of observations above t
//   */
//  double probability(double z, double t, int n, int Nt) const;

//  /**
//   *  \brief Given some extra parameters, compute the quantile z such that
//   F(z) = 1-q
//   *  \param q Desired probability
//   *  \param t Intermediate threshold
//   *  \param n Total number of observations
//   *  \param Nt Total number of observations above t
//   */
//  double quantile(double q, double t, int n, int Nt) const;
// };

#endif // PEAKS_H
