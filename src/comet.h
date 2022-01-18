#ifndef COMET_H
#define COMET_H

#include <libnova/ln_types.h>
#include <libnova/julian_day.h>
#include <libnova/elliptic_motion.h>
#include <libnova/parabolic_motion.h>
#include <libnova/hyperbolic_motion.h>
#include <libnova/comet.h>
#include "celestialbody.h"
#include "orbit.h"
#include "mpcparser.h"

class Comet : public CelestialBody, public Orbit
{
public:
    Comet(struct comet_t &comet, double JD);

    void updatePosition(double JD) override;
    void updateOrbit() override;
    double magnitude() const;
    double distEarth() const;
    double distSun() const;
    static double orbitLengthJd();

private:
    struct comet_t m_comet;
    struct ln_ell_orbit m_ell_orbit;
    struct ln_par_orbit m_par_orbit;
    struct ln_hyp_orbit m_hyp_orbit;
    double m_magnitude;
    double m_distEarth;
    double m_distSun;
};

#endif // COMET_H
