#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISCalendarDayWidget : public QWidget
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
