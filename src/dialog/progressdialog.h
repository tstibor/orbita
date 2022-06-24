#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QProgressDialog>
#include <QLabel>
#include "logger.h"

class ProgressDialog : public QProgressDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();

public slots:
    void progress(qreal percent, quint32 linesParsed);

private:
    QLabel *m_Label;
};

#endif // PROGRESSDIALOG_H
