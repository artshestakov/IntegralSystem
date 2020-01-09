#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTabWidgetObject : public QTabWidget
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
