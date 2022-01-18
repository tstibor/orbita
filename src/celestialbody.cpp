#include "celestialbody.h"

CelestialBody::CelestialBody()
{
    memset(&m_position, 0, sizeof(m_position));
    m_JD = 0;
    m_name = "";
}

CelestialBody::CelestialBody(QString &name, struct ln_rect_posn &position, double JD)
    : m_name(name), m_position(position), m_JD(JD)
{
}

QString CelestialBody::name() const
{
    return m_name;
}

struct ln_rect_posn CelestialBody::position() const
{
    return m_position;
}

double CelestialBody::JD() const
{
    return m_JD;
}

void CelestialBody::setPosition(struct ln_rect_posn &position)
{
    m_position = position;
}

void CelestialBody::setJD(double JD)
{
    m_JD = JD;
}

void CelestialBody::setName(QString &name)
{
    m_name = name;
}
