#pragma once
#ifndef _EVMAINWINDOW_H_INCLUDED
#define _EVMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtWidgets/QWidget>
//-----------------------------------------------------------------------------
class EVMainWindow : public QWidget
{
	Q_OBJECT

public:
	EVMainWindow(const QString &PathCrashFile, QWidget *parent = 0);
	virtual ~EVMainWindow();
};
//-----------------------------------------------------------------------------
#endif
