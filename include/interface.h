/**
        @file interface.h
        @brief C interface header (for python notably)
        @author Alban Siffer
        @date 03 March 2018
*/

#include "dspot.h"

using namespace std;

extern "C" {
// ----------------------------------------------------------------------------
// SPOT
// ----------------------------------------------------------------------------
Spot* Spot_new(double q,
    int n_init,
    double level,
    bool up,
    bool down,
    bool alert,
    bool bounded,
    int max_excess);

void Spot_delete(Spot *s);

int Spot_step(Spot* s, double x);

SpotStatus Spot_status(Spot* s);

SpotConfig Spot_config(Spot* s);

double Spot_getUpperThreshold(Spot* s);

double Spot_getLowerThreshold(Spot* s);

double Spot_getUpper_t(Spot* s);

double Spot_getLower_t(Spot* s);

void Spot_set_q(Spot* s, double q_new);

double Spot_up_probability(Spot* s, double z);

double Spot_down_probability(Spot* s, double z);

// ----------------------------------------------------------------------------
// DSPOT
// ----------------------------------------------------------------------------

DSpot* DSpot_new(int d,
    double q,
    int n_init,
    double level,
    bool up,
    bool down,
    bool alert,
    bool bounded,
    int max_excess);

void DSpot_delete(DSpot* ds);

DSpotStatus DSpot_status(DSpot* ds);

DSpotConfig DSpot_config(DSpot* ds);

int DSpot_step(DSpot* ds, double x);

double DSpot_getUpperThreshold(DSpot* ds);

double DSpot_getLowerThreshold(DSpot* ds);

double DSpot_getUpper_t(DSpot* ds);

double DSpot_getLower_t(DSpot* ds);

double DSpot_getDrift(DSpot* ds);

void DSpot_set_q(DSpot* ds, double q_new);

double DSpot_up_probability(DSpot* ds, double z);

double DSpot_down_probability(DSpot* ds, double z);

}
