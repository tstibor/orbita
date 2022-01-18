#include "orbit.h"

Orbit::Orbit(double periodDays)
    : m_periodDays(periodDays)
{}

double Orbit::periodDays() const
{
    return m_periodDays;
}

void Orbit::setPeriodDays(double periodDays)
{
    m_periodDays = periodDays;
}

const QVector<struct ln_rect_posn> &Orbit::orbit() const
{
    return m_orbit;
}

void Orbit::clearOrbit()
{
    m_orbit.clear();
}

void Orbit::addPositionOrbit(struct ln_rect_posn position)
{
    m_orbit.push_back(position);
}
