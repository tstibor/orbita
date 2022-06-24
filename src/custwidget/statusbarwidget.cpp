#include "statusbarwidget.h"

StatusbarWidget::StatusbarWidget(QWidget *parent) : QWidget(parent)
{
    m_LabelDate = new QLabel(this);
    m_LabelDate->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_LabelProjection = new QLabel(this);
    m_LabelProjection->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QFrame *frameVLine1 = new QFrame(this);
    frameVLine1->setFrameShape(QFrame::VLine);
    QFrame *frameVLine2 = new QFrame(this);
    frameVLine2->setFrameShape(QFrame::VLine);

    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->setSpacing(5);
    hboxLayout->setSizeConstraint(QLayout::SetMinimumSize);
    hboxLayout->setContentsMargins(0, 0, 0, 0);

    hboxLayout->addWidget(m_LabelDate);
    hboxLayout->addWidget(frameVLine1);
    hboxLayout->addWidget(m_LabelProjection);
    hboxLayout->addWidget(frameVLine2);
    setLayout(hboxLayout);

    updateProjection(true);
}

void StatusbarWidget::updateDate(double JD)
{
    m_LabelDate->setText(tr("JD") + ": " + QString::number(JD, 'f', 2));
    m_LabelDate->update();
}

void StatusbarWidget::updateProjection(bool isOrthographic)
{
    const QString persp = isOrthographic ? tr("Orthographic") : tr("Perspective");
    m_LabelProjection->setText(tr("Projection") + ": " + persp);
}
