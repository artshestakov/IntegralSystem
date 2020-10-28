#pragma once
#ifndef _LVMAINWINDOW_H_INCLUDED
#define _LVMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTabWidget>
//-----------------------------------------------------------------------------
class LVMainWindow : public QWidget
{
    Q_OBJECT

public:
    LVMainWindow(QWidget *parent = 0);
	virtual ~LVMainWindow();

private:
	void OpenFile();
};
//-----------------------------------------------------------------------------
#endif
