#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QSplitter>
#include <libnova/ln_types.h>
#include <libnova/julian_day.h>
#include "common.h"

class StatusbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusbarWidget(QWidget *parent = nullptr);

private:
    QLabel *m_LabelDate;
    QLabel *m_LabelProjection;

public slots:
    void updateDate(double JD);
    void updateProjection(bool isOrthographic);
};

#endif // STATUSBARWIDGET_H
