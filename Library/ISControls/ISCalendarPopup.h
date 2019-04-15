#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
#include "ISCalendarWidget.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISCalendarPopup : public ISCalendarWidget
{
	Q_OBJECT

public:
	ISCalendarPopup(QWidget *parent = 0);
	virtual ~ISCalendarPopup();
};
//-----------------------------------------------------------------------------
