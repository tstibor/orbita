#ifndef ASTEROID_H
#define ASTEROID_H

#include <libnova/ln_types.h>
#include <libnova/julian_day.h>
#include <libnova/asteroid.h>
#include "celestialbody.h"
#include "orbit.h"
#include "mpcparser.h"

class Asteroid : public CelestialBody, public Orbit
{
public:
    Asteroid(struct asteroid_t &asteroid, double JD);

    void updatePosition(double JD) override;
    void updateOrbit() override;
    double magnitude() const;
    double distEarth() const;
    double distSun() const;

private:
    struct asteroid_t m_asteroid;
    struct ln_ell_orbit m_orbit;
    double m_magnitude;
    double m_distEarth;
    double m_distSun;

    double orbitalPeriod();
};

#endif // ASTEROID_H
