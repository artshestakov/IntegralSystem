#pragma once
#ifndef _LVMAINWINDOW_H_INCLUDED
#define _LVMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class LVMainWindow : public ISInterfaceForm
{
    Q_OBJECT

public:
    LVMainWindow(QWidget *parent = 0);
	virtual ~LVMainWindow();

private:
	void OpenFile();

private:
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
#endif
