#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISTabWidgetObject : public QTabWidget
{
	Q_OBJECT

public:
	ISTabWidgetObject(QWidget *parent = 0);
	virtual ~ISTabWidgetObject();

protected:
	void tabInserted(int Index);
	void CloseTab();
};
//-----------------------------------------------------------------------------
