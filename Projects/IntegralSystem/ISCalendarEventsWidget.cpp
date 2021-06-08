#include "ISCalendarEventsWidget.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISCalendarEventsWidget::ISCalendarEventsWidget(QWidget *parent) : ISListWidget(parent)
{
    setFrameShape(QFrame::NoFrame);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(model(), &QAbstractItemModel::rowsInserted, this, &ISCalendarEventsWidget::RowsInserted);
    connect(model(), &QAbstractItemModel::rowsRemoved, this, &ISCalendarEventsWidget::RowsRemoved);

    LabelEmpty = new QLabel(this);
    LabelEmpty->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
    LabelEmpty->setFont(FONT_TAHOMA_12_BOLD);
    LabelEmpty->setText(LANG("NotEventsWithSelectedDay"));
    LabelEmpty->setWordWrap(true);
    LabelEmpty->setAlignment(Qt::AlignCenter);
}
//-----------------------------------------------------------------------------
ISCalendarEventsWidget::~ISCalendarEventsWidget()
{

}
//-----------------------------------------------------------------------------
void ISCalendarEventsWidget::paintEvent(QPaintEvent *e)
{
    ISListWidget::paintEvent(e);

    if (!count())
    {
        QRect Rect = frameGeometry();
        QPoint CenterPoint = Rect.center();
        CenterPoint.setX(CenterPoint.x() - (LabelEmpty->width() / 2));
        CenterPoint.setY(CenterPoint.y() - (LabelEmpty->height() / 2));
        LabelEmpty->move(CenterPoint);
    }
}
//-----------------------------------------------------------------------------
void ISCalendarEventsWidget::RowsInserted(const QModelIndex &ModelIndex, int First, int Last)
{
    Q_UNUSED(ModelIndex);
    Q_UNUSED(First);
    Q_UNUSED(Last);
    LabelEmpty->setVisible(!count());
}
//-----------------------------------------------------------------------------
void ISCalendarEventsWidget::RowsRemoved(const QModelIndex &ModelIndex, int First, int Last)
{
    Q_UNUSED(ModelIndex);
    Q_UNUSED(First);
    Q_UNUSED(Last);
    LabelEmpty->setVisible(!count());
}
//-----------------------------------------------------------------------------
