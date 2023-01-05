#ifndef ORBIT_H
#define ORBIT_H

#include <QVector>
#include <libnova/ln_types.h>

constexpr double daysOneYear = 365.2568984;

class Orbit
{

public:
    Orbit(double periodDays);
    double periodDays() const;
    void setPeriodDays(double periodDays);
    const QVector<struct ln_rect_posn> &orbit() const;
    virtual void updateOrbit() = 0;
    void clearOrbit();

protected:
    void addPositionOrbit(struct ln_rect_posn position);

private:
    double m_periodDays;
    QVector<struct ln_rect_posn> m_orbit;
};

#endif // ORBIT_H
