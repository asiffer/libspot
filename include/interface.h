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


// ----------------------------------------------------------------------------
// SPOT STATUS
// ----------------------------------------------------------------------------
SpotStatus* Spot_status_ptr(Spot* s);

void Spot_status_delete(SpotStatus* ss);

// ----------------------------------------------------------------------------
// DSPOT STATUS
// ----------------------------------------------------------------------------
DSpotStatus* DSpot_status_ptr(DSpot* ds);

void DSpot_status_delete(DSpotStatus* ds);


// ----------------------------------------------------------------------------
// SPOT/DSPOT status methods
// ----------------------------------------------------------------------------
int _status_get_n(SpotStatus* ss);

int _status_get_ex_up(SpotStatus* ss);

int _status_get_ex_down(SpotStatus* ss);

int _status_get_Nt_up(SpotStatus* ss);

int _status_get_Nt_down(SpotStatus* ss);

int _status_get_al_up(SpotStatus* ss);

int _status_get_al_down(SpotStatus* ss);

double _status_get_t_up(SpotStatus* ss);

double _status_get_t_down(SpotStatus* ss);

double _status_get_z_up(SpotStatus* ss);

double _status_get_z_down(SpotStatus* ss);

// specific to DSpot
double _status_get_drift(DSpotStatus* dss);

// ----------------------------------------------------------------------------
// SPOT CONFIG
// ----------------------------------------------------------------------------
SpotConfig* Spot_config_ptr(Spot* s);

void Spot_config_delete(SpotConfig* sc);

// ----------------------------------------------------------------------------
// DSPOT CONFIG
// ----------------------------------------------------------------------------
DSpotConfig* DSpot_config_ptr(DSpot* ds);

void DSpot_config_delete(DSpotConfig* dsc);


// ----------------------------------------------------------------------------
// SPOT/DSPOT config methods
// ----------------------------------------------------------------------------

double _config_get_q(SpotConfig* sc);

int _config_get_bounded(SpotConfig* sc);

int _config_get_max_excess(SpotConfig* sc);

int _config_get_alert(SpotConfig* sc);

int _config_get_up(SpotConfig* sc);

int _config_get_down(SpotConfig* sc);

int _config_get_n_init(SpotConfig* sc);

double _config_get_level(SpotConfig* sc);

// specific to DSpot
double _config_get_depth(DSpotConfig* dsc);
