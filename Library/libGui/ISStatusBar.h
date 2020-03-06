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

public:
	ISStatusBar(QWidget *parent = 0);
	virtual ~ISStatusBar();
};
//-----------------------------------------------------------------------------
