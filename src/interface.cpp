/**
        @file interface.cpp
        @brief C interface implementation
        @author Alban Siffer
        @date 03 March 2018
*/

// #include "dspot.h"
#include "interface.h"

using namespace std;

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
    int max_excess)
{
    return new Spot(q, n_init, level, up, down, alert, bounded, max_excess);
}

void Spot_delete(Spot* s)
{
    delete s;
}

int Spot_step(Spot* s, double x)
{
    return s->step(x);
}

SpotStatus Spot_status(Spot* s)
{
    return s->status();
}

SpotConfig Spot_config(Spot* s)
{
    return s->config();
}

double Spot_getUpperThreshold(Spot* s)
{
    return s->getUpperThreshold();
}

double Spot_getLowerThreshold(Spot* s)
{
    return s->getLowerThreshold();
}

double Spot_getUpper_t(Spot* s)
{
    return s->getUpper_t();
}

double Spot_getLower_t(Spot* s)
{
    return s->getLower_t();
}

void Spot_set_q(Spot* s, double q_new)
{
    s->set_q(q_new);
}

double Spot_up_probability(Spot* s, double z)
{
    return s->up_probability(z);
}

double Spot_down_probability(Spot* s, double z)
{
    return s->down_probability(z);
}


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
    int max_excess)
{
    return new DSpot(d, q, n_init, level, up, down, alert, bounded, max_excess);
}

void DSpot_delete(DSpot* ds)
{
    delete ds;
}

DSpotStatus DSpot_status(DSpot* ds)
{
    return ds->status();
}

DSpotConfig DSpot_config(DSpot* ds)
{
    return ds->config();
}

int DSpot_step(DSpot* ds, double x)
{
    return ds->step(x);
}

double DSpot_getUpperThreshold(DSpot* ds)
{
    return ds->getUpperThreshold();
}

double DSpot_getLowerThreshold(DSpot* ds)
{
    return ds->getLowerThreshold();
}

double DSpot_getUpper_t(DSpot* ds)
{
    return ds->getUpper_t();
}

double DSpot_getLower_t(DSpot* ds)
{
    return ds->getLower_t();
}

double DSpot_getDrift(DSpot* ds)
{
    return ds->getDrift();
}

void DSpot_set_q(DSpot* ds, double q_new)
{
    ds->set_q(q_new);
}

double DSpot_up_probability(DSpot* ds, double z)
{
    return ds->up_probability(z);
}

double DSpot_down_probability(DSpot* ds, double z)
{
    return ds->down_probability(z);
}




// ----------------------------------------------------------------------------
// SPOT STATUS
// ----------------------------------------------------------------------------
SpotStatus* Spot_status_ptr(Spot* s) 
{
    return new SpotStatus{s->status()};
}

void Spot_status_delete(SpotStatus* ss)
{
    delete ss;
}


// ----------------------------------------------------------------------------
// DSPOT STATUS
// ----------------------------------------------------------------------------
DSpotStatus* DSpot_status_ptr(DSpot* ds) 
{
    return new DSpotStatus{ds->status()};
}

void DSpot_status_delete(DSpotStatus* ds)
{
    delete ds;
}

// ----------------------------------------------------------------------------
// SPOT/DSPOT status methods
// ----------------------------------------------------------------------------
int _status_get_n(SpotStatus* ss) 
{
    return ss->n;
}

int _status_get_ex_up(SpotStatus* ss) 
{
    return ss->ex_up;
}

int _status_get_ex_down(SpotStatus* ss) 
{
    return ss->ex_down;
}

int _status_get_Nt_up(SpotStatus* ss) 
{
    return ss->Nt_up;
}

int _status_get_Nt_down(SpotStatus* ss) 
{
    return ss->Nt_down;
}

int _status_get_al_up(SpotStatus* ss) 
{
    return ss->al_up;
}

int _status_get_al_down(SpotStatus* ss) 
{
    return ss->al_down;
}

double _status_get_t_up(SpotStatus* ss) 
{
    return ss->t_up;
}

double _status_get_t_down(SpotStatus* ss) 
{
    return ss->t_down;
}

double _status_get_z_up(SpotStatus* ss) 
{
    return ss->z_up;
}

double _status_get_z_down(SpotStatus* ss) 
{
    return ss->z_down;
}

// specific to DSpot
double _status_get_drift(DSpotStatus* dss) 
{
    return dss->drift;
}


// ----------------------------------------------------------------------------
// SPOT CONFIG
// ----------------------------------------------------------------------------

SpotConfig* Spot_config_ptr(Spot* s) 
{
    return new SpotConfig{s->config()};
}

void Spot_config_delete(SpotConfig* sc)
{
    delete sc;
}

// ----------------------------------------------------------------------------
// DSPOT CONFIG
// ----------------------------------------------------------------------------
DSpotConfig* DSpot_config_ptr(DSpot* ds) 
{
    return new DSpotConfig{ds->config()};
}

void DSpot_config_delete(DSpotConfig* dsc)
{
    delete dsc;
}


// ----------------------------------------------------------------------------
// SPOT/DSPOT config methods
// ----------------------------------------------------------------------------

double _config_get_q(SpotConfig* sc)
{
    return sc->q;
}

int _config_get_bounded(SpotConfig* sc)
{
    if (sc->bounded) 
    {
        return 1;
    }
    return 0;
}

int _config_get_max_excess(SpotConfig* sc)
{
    return sc->max_excess;
}

int _config_get_alert(SpotConfig* sc)
{
    if (sc->alert) 
    {
        return 1;
    }
    return 0;
}

int _config_get_up(SpotConfig* sc)
{
    if (sc->up) 
    {
        return 1;
    }
    return 0;
}

int _config_get_down(SpotConfig* sc)
{
    if (sc->down) 
    {
        return 1;
    }
    return 0;
}

int _config_get_n_init(SpotConfig* sc)
{
    return sc->n_init;
}

double _config_get_level(SpotConfig* sc)
{
    return sc->level;
}

// specific to DSpot
double _config_get_depth(DSpotConfig* dsc) 
{
    return dsc->depth;
}