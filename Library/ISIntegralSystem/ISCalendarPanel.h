#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISCalendarWidget.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISCalendarPanel : public ISCalendarWidget
{
	Q_OBJECT

public:
	ISCalendarPanel(QWidget *parent = 0);
	virtual ~ISCalendarPanel();

	void UpdateCells();

protected:
	void paintCell(QPainter *Painter, const QRect &Rect, const QDate &Date) const;
	void CurrentPageChanged(int Year, int Month);

private:
	QMap<QDate, int> DaysEvent; //Список дат, по которым числятся события
	QPixmap PixmapIndicator;
};
//-----------------------------------------------------------------------------
