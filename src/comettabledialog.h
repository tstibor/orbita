#ifndef COMETTABLEDIALOG_H
#define COMETTABLEDIALOG_H

#include "tabledialog.h"

class CometTableDialog : public TableDialog
{
    Q_OBJECT

public:
    explicit CometTableDialog(QWidget *parent = nullptr, SolarSystem *solarSystem = nullptr);

protected:
    void renderSelection() override;

signals:
    void selectedComet(struct comet_t &comet);
    void displaySelectedComets();
};

#endif // COMETTABLEDIALOG_H
