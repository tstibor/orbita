#include "asteroid.h"

Asteroid::Asteroid(struct asteroid_t &asteroid, double JD) :
    CelestialBody(), Orbit(0)
{
    memset(&m_orbit, 0, sizeof(m_orbit));
    memcpy(&m_asteroid, &asteroid, sizeof(asteroid));

    QString name(m_asteroid.designation_readable);

    setName(name);
    m_orbit.a = m_asteroid.semimajor_axis;
    m_orbit.e = m_asteroid.eccentricity;
    m_orbit.i = m_asteroid.inclination;
    m_orbit.omega = m_asteroid.ascending_node;
    m_orbit.w = m_asteroid.perihelion;
    m_orbit.n = m_asteroid.daily_motion;
    m_orbit.JD = ln_get_ell_last_perihelion(ln_get_julian_from_mpc(m_asteroid.epoch),
					    m_asteroid.anomaly_mean, m_orbit.n);

    updatePosition(JD);
    setPeriodDays(orbitalPeriod());
}

void Asteroid::updatePosition(double JD)
{
    struct ln_rect_posn pos = {.X = 0, .Y = 0, .Z = 0};

    setJD(JD);
    ln_get_ell_helio_rect_posn(&m_orbit, JD, &pos);
    setPosition(pos);
    m_magnitude = ln_get_asteroid_mag(JD, &m_orbit, m_asteroid.H, m_asteroid.G);
    m_distSun = ln_get_ell_body_solar_dist(JD, &m_orbit);
    m_distEarth = ln_get_ell_body_earth_dist(JD, &m_orbit);
}

void Asteroid::updateOrbit()
{
    struct ln_rect_posn pos = {.X = 0, .Y = 0, .Z = 0};
    const double stepsJD = periodDays() / 100.0;

    for (auto jd = JD(); jd <= JD() + periodDays(); jd += stepsJD) {
	ln_get_ell_helio_rect_posn(&m_orbit, jd, &pos);
	addPositionOrbit(pos);
    }
}

double Asteroid::orbitalPeriod()
{
    return 365.0 * pow(m_orbit.a, 1.5);
}

double Asteroid::magnitude() const
{
    return m_magnitude;
}

double Asteroid::distEarth() const
{
    return m_distEarth;
}

double Asteroid::distSun() const
{
    return m_distSun;
}
