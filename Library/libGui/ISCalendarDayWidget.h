#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCalendarDayWidget : public QWidget
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
