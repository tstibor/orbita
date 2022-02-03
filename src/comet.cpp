#include "comet.h"

Comet::Comet(struct comet_t &comet, double JD) :
    CelestialBody(), Orbit(0)
{
    struct ln_date epoch_date = {
	.years = comet.perihelion_year,
	.months = comet.perihelion_month,
	.days = 0,
	.hours = 0,
	.minutes = 0,
	.seconds = 0
    };

    memset(&m_ell_orbit, 0, sizeof(m_ell_orbit));
    memset(&m_par_orbit, 0, sizeof(m_par_orbit));
    memset(&m_hyp_orbit, 0, sizeof(m_hyp_orbit));
    memcpy(&m_comet, &comet, sizeof(comet));

    QString name(m_comet.designation_name);

    setName(name);

    /* Orbital eccentricity. */
    m_ell_orbit.e = m_hyp_orbit.e = m_comet.eccentricity;
    /* Perihelion distance in AU. */
    m_par_orbit.q = m_hyp_orbit.q = m_comet.perihelion_distance;
    /* Inclination in degrees. */
    m_ell_orbit.i = m_par_orbit.i = m_hyp_orbit.i = m_comet.inclination;
    /* Longitude of ascending node in degrees. */
    m_ell_orbit.omega = m_par_orbit.omega = m_hyp_orbit.omega = m_comet.ascending_node;
    /* Argument of perihelion in degrees. */
    m_ell_orbit.w = m_par_orbit.w = m_hyp_orbit.w = m_comet.perihelion;
    /* Time of last passage in Perihelion, in julian day. */
    m_ell_orbit.JD = m_par_orbit.JD = m_hyp_orbit.JD = ln_get_julian_day(&epoch_date)
	+ comet.perihelion_day;

    updatePosition(JD);
}

void Comet::updatePosition(double JD)
{
    struct ln_rect_posn pos = {.X = 0, .Y = 0, .Z = 0};

    setJD(JD);
    ln_get_par_helio_rect_posn(&m_par_orbit, JD, &pos);
    setPosition(pos);
    m_magnitude = ln_get_par_comet_mag(JD, &m_par_orbit, m_comet.H, 2.5 * m_comet.G);
    m_distSun = ln_get_par_body_solar_dist(JD, &m_par_orbit);
    m_distEarth = ln_get_par_body_earth_dist(JD, &m_par_orbit);
}

void Comet::updateOrbit()
{
    struct ln_rect_posn pos = {.X = 0, .Y = 0, .Z = 0};
    const double stepsJD = 1.0;
    const double orbitFromJd = JD() - Comet::orbitLengthJd();
    const double orbitToJd = JD() + Comet::orbitLengthJd();

    for (auto jd = orbitFromJd; jd <= orbitToJd; jd += stepsJD) {
	ln_get_par_helio_rect_posn(&m_par_orbit, jd, &pos);
	addPositionOrbit(pos);
    }
}

double Comet::magnitude() const
{
    return m_magnitude;
}

double Comet::distEarth() const
{
    return m_distEarth;
}

double Comet::distSun() const
{
    return m_distSun;
}

double Comet::orbitLengthJd()
{
    return 365.25 * 25.0; /* 25 years. */
}
