#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCalendarDayWidget : public QWidget
{
	Q_OBJECT

public:
	ISCalendarDayWidget(QWidget *parent = 0);
	virtual ~ISCalendarDayWidget();

	void SetSelectedDate(const QDate &Date);

private:
	QLabel *LabelDayNumber;
	QLabel *LabelDayName;
	QLabel *LabelMonthYear;
};
//-----------------------------------------------------------------------------
