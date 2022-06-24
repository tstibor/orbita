#ifndef ASTEROIDTABLEDIALOG_H
#define ASTEROIDTABLEDIALOG_H

#include "tabledialog.h"

class AsteroidTableDialog : public TableDialog
{
    Q_OBJECT

public:
    explicit AsteroidTableDialog(QWidget *parent = nullptr, SolarSystem *solarSystem = nullptr);

protected:
    void renderSelection() override;

signals:
    void selectedAsteroid(struct asteroid_t &asteroid);
    void displaySelectedAsteroids();
};

#endif // ASTEROIDTABLEDIALOG_H
