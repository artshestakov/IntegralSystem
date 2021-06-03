#pragma once
//-----------------------------------------------------------------------------
#include "ISCalendarWidget.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCalendarPanel : public ISCalendarWidget
{
    Q_OBJECT

public:
    ISCalendarPanel(QWidget *parent = 0);
    virtual ~ISCalendarPanel();

    void SetDays(const ISVectorUInt &days);

protected:
    void paintCell(QPainter *Painter, const QRect &Rect, const QDate &Date) const;
    void CurrentPageChanged(int Year, int Month);

private:
    ISVectorUInt Days;
    QPixmap PixmapIndicator;
};
//-----------------------------------------------------------------------------
