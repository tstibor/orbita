#ifndef PLANET_H
#define PLANET_H

#include <QColor>
#include <libnova/ln_types.h>
#include <libnova/julian_day.h>
#include <libnova/solar.h>
#include <libnova/mercury.h>
#include <libnova/venus.h>
#include <libnova/earth.h>
#include <libnova/mars.h>
#include <libnova/jupiter.h>
#include <libnova/saturn.h>
#include <libnova/uranus.h>
#include <libnova/neptune.h>
#include <libnova/pluto.h>
#include "celestialbody.h"
#include "orbit.h"

class Planet : public CelestialBody, public Orbit
{

public:
    Planet(QString name, struct ln_rect_posn position, double periodDays, double JD, double radiusMeter,
           QColor color);
    double radiusMeter() const;
    double radiusAU() const;
    QColor color() const;
    void updatePosition(double JD) override;
    void updateOrbit() override;

private:
    double m_radiusMeter;
    QColor m_color;

    void (*ln_get_rect_helio)(double JD, struct ln_rect_posn *position);
};

#endif // PLANET_H
