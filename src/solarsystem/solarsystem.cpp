#include "solarsystem.h"

SolarSystem::SolarSystem(double JD) : m_JD(JD), m_nextOrbitCometUpdateJD(JD)
{
    /* name, cartesian position, orbital period [days], julian date, radius [meter], RGB color. */
    m_planets.append(Planet("mercury", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    87.9691, m_JD, 2439700, QColor(151, 151, 159)));
    m_planets.append(Planet("venus", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    224.701, m_JD, 6051800, QColor(211, 156, 126)));
    m_planets.append(Planet("earth", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    365.256, m_JD, 6371000, QColor(140, 177, 222)));
    m_planets.append(Planet("mars", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    686.971, m_JD, 3389500, QColor(198, 123, 92)));
    m_planets.append(Planet("jupiter", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    4332.59, m_JD, 69911000, QColor(211, 156, 126)));
    m_planets.append(Planet("saturn", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    10759.22, m_JD, 58232000, QColor(197, 171, 110)));
    m_planets.append(Planet("uranus", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    30688.5, m_JD, 25362000, QColor(62, 84, 232)));
    m_planets.append(Planet("neptune", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    60182, m_JD, 24622000, QColor(213, 251, 252)));
    m_planets.append(Planet("pluto", (struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
			    90560, m_JD, 1188300, QColor(255, 241, 213)));
}

const QVector<Planet> &SolarSystem::planets() const
{
    return m_planets;
}

const QVector<Asteroid> &SolarSystem::asteroids() const
{
    return m_asteroids;
}

const QVector<Comet> &SolarSystem::comets() const
{
    return m_comets;
}

void SolarSystem::computePlanetsPosition()
{
    for (auto &planet : m_planets)
        planet.updatePosition(m_JD);

    emit planetsPositionChanged(m_JD);
}

void SolarSystem::computePlanetsOrbit()
{
    for (auto &planet : m_planets) {
        planet.clearOrbit();
        planet.updateOrbit();
    }

    emit planetsOrbitChanged(m_JD);
}

void SolarSystem::computeAsteroidsPosition()
{
    for (auto &asteroid : m_asteroids)
        asteroid.updatePosition(m_JD);

    emit asteroidsPositionChanged(m_JD);
}

void SolarSystem::computeAsteroidsOrbit()
{
    for (auto &asteroid : m_asteroids) {
        asteroid.clearOrbit();
        asteroid.updateOrbit();
    }

    emit asteroidsOrbitChanged(m_JD);
}

void SolarSystem::computeCometsPosition()
{
    for (auto &comet : m_comets)
        comet.updatePosition(m_JD);

    emit cometsPositionChanged(m_JD);
}

void SolarSystem::computeCometsOrbit()
{
    for (auto &comet : m_comets) {
        comet.clearOrbit();
        comet.updateOrbit();
    }

    emit cometsOrbitChanged(m_JD);
}

double SolarSystem::JD() const
{
    return m_JD;
}

void SolarSystem::setJD(double JD)
{
    m_JD = JD;

    computePlanetsPosition();
    computeAsteroidsPosition();
    computeCometsPosition();

    emit positionsChanged(JD);

    if (qFabs(m_nextOrbitCometUpdateJD - JD) > Comet::orbitLengthJd()) {
	m_nextOrbitCometUpdateJD = JD;
	computeCometsOrbit();
    }
}

void SolarSystem::addAsteroid(Asteroid &asteroid)
{
    m_asteroids.append(asteroid);
}

void SolarSystem::removeAsteroids()
{
    m_asteroids.clear();
    emit asteroidsPositionChanged(m_JD);
}

void SolarSystem::addComet(Comet &comet)
{
    m_comets.append(comet);
}

void SolarSystem::removeComets()
{
    m_comets.clear();
    emit cometsPositionChanged(m_JD);
}
