#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISTabWidgetObject : public QTabWidget
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
