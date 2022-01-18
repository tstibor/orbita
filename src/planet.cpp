#include "planet.h"

Planet::Planet(QString name, struct ln_rect_posn position, double periodDays,
	       double JD, double radiusMeter, QColor color)
    : CelestialBody(name, position, JD), Orbit(periodDays), m_radiusMeter(radiusMeter), m_color(color)
{
    if (name == "mercury")
	ln_get_rect_helio = ln_get_mercury_rect_helio;
    else if (name == "venus")
	ln_get_rect_helio = ln_get_venus_rect_helio;
    else if (name == "earth")
	ln_get_rect_helio = ln_get_earth_rect_helio;
    else if (name == "mars")
	ln_get_rect_helio = ln_get_mars_rect_helio;
    else if (name == "jupiter")
	ln_get_rect_helio = ln_get_jupiter_rect_helio;
    else if (name == "saturn")
	ln_get_rect_helio = ln_get_saturn_rect_helio;
    else if (name == "uranus")
	ln_get_rect_helio = ln_get_uranus_rect_helio;
    else if (name == "neptune")
	ln_get_rect_helio = ln_get_neptune_rect_helio;
    else if (name == "pluto")
	ln_get_rect_helio = ln_get_pluto_rect_helio;
    else
	return;

    updatePosition(JD);
}

double Planet::radiusMeter() const
{
    return m_radiusMeter;
}

double Planet::radiusAU() const
{
    return m_radiusMeter / ONE_AU_IN_METER;
}

QColor Planet::color() const
{
    return m_color;
}

void Planet::updatePosition(double JD)
{
    struct ln_rect_posn pos = {.X = 0, .Y = 0, .Z = 0};

    setJD(JD);
    ln_get_rect_helio(JD, &pos);
    setPosition(pos);
}

void Planet::updateOrbit()
{
    struct ln_rect_posn pos = {.X = 0, .Y = 0, .Z = 0};
    const double stepsJD = periodDays() / 100.0;

    for (auto jd = JD(); jd <= JD() + periodDays(); jd += stepsJD) {
	ln_get_rect_helio(jd, &pos);
	addPositionOrbit(pos);
    }
}
