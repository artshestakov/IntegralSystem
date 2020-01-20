#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISCalendarWidget.h"
//-----------------------------------------------------------------------------
class ISCalendarPopup : public ISCalendarWidget
{
	Q_OBJECT

public:
	ISCalendarPopup(QWidget *parent = 0);
	virtual ~ISCalendarPopup();
};
//-----------------------------------------------------------------------------
