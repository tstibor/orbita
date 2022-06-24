#include "progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) : QProgressDialog(parent)
{
    m_Label = new QLabel(this);
    m_Label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_Label->setText(tr("Total processed celestial objects: 0000000"));
    m_Label->setMinimumSize(m_Label->sizeHint());
    setLabel(m_Label);

}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::progress(qreal percent, quint32 linesParsed)
{
    if (!wasCanceled()) {
	setValue(percent);
	if (linesParsed % 1000 == 0)
	    setLabelText(QString(tr("Total processed celestial objects: %1").arg(linesParsed)));
    }
}
