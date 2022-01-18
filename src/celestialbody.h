#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#include <QString>
#include <libnova/ln_types.h>
#include <libnova/elliptic_motion.h>
#include "common.h"
#include "logger.h"

class CelestialBody
{
public:
    CelestialBody();
    CelestialBody(QString &name, struct ln_rect_posn &position, double JD);

    QString name() const;
    struct ln_rect_posn position() const;
    double JD() const;
    operator QString() const
    {
        return QString("%1 jd:%2 pos:(%3,%4,%5)")
                .arg(name())
                .arg(JD(), 0, 'f', 2)
                .arg(position().X)
                .arg(position().Y)
                .arg(position().Z);
    };

protected:
    virtual void updatePosition(double JD) = 0;
    void setPosition(struct ln_rect_posn &position);
    void setName(QString &name);
    void setJD(double JD);

private:
    QString m_name;
    struct ln_rect_posn m_position;
    double m_JD;
};

#endif // CELESTIALBODY_H
