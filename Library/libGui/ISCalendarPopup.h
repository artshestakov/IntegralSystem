#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISCalendarWidget.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCalendarPopup : public ISCalendarWidget
{
	Q_OBJECT

public:
	ISCalendarPopup(QWidget *parent = 0);
	virtual ~ISCalendarPopup();
};
//-----------------------------------------------------------------------------
