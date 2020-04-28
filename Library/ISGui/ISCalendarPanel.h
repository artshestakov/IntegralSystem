#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISCalendarWidget.h"
//-----------------------------------------------------------------------------
class ISCalendarPanel : public ISCalendarWidget
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
	QMap<QDate, int> DaysEvent; //������ ���, �� ������� �������� �������
	QPixmap PixmapIndicator;
};
//-----------------------------------------------------------------------------