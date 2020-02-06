#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"

//-----------------------------------------------------------------------------
class ISStatusBar : public QStatusBar
{
	Q_OBJECT

signals:
	void DateTimeClicked();
	void MakeCall();
	void TrayClicked();

public:
	ISStatusBar(QWidget *parent = 0);
	virtual ~ISStatusBar();
};
//-----------------------------------------------------------------------------
