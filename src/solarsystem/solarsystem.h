#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include <QObject>
#include <QVector>
#include "planet.h"
#include "asteroid.h"
#include "comet.h"

class SolarSystem : public QObject
{
    Q_OBJECT

public:
    SolarSystem(double m_JD = ln_get_julian_from_sys());
    const QVector<Planet> &planets() const;
    const QVector<Asteroid> &asteroids() const;
    const QVector<Comet> &comets() const;

    void computePlanetsPosition();
    void computePlanetsOrbit();
    void computeAsteroidsPosition();
    void computeAsteroidsOrbit();
    void computeCometsPosition();
    void computeCometsOrbit();
    double JD() const;

private:
    double m_JD;
    QVector<Planet> m_planets;
    QVector<Asteroid> m_asteroids;
    QVector<Comet> m_comets;
    double m_nextOrbitCometUpdateJD;

signals:
    void positionsChanged(double JD);
    void planetsPositionChanged(double JD);
    void planetsOrbitChanged(double JD);
    void asteroidsPositionChanged(double JD);
    void asteroidsOrbitChanged(double JD);
    void cometsPositionChanged(double JD);
    void cometsOrbitChanged(double JD);

public slots:
    void setJD(double JD);
    void addAsteroid(Asteroid &asteroid);
    void addComet(Comet &comet);
    void removeAsteroids();
    void removeComets();
};

#endif // SOLARSYSTEM_H
