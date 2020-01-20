#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISSplashWidget : public QFrame
{
	Q_OBJECT

public:
    ISSplashWidget(QWidget *parent = 0);
	virtual ~ISSplashWidget();

	void showEvent(QShowEvent *e);
	void SetText(const QString &Text);

private:
	QLabel *LabelText;
};
//-----------------------------------------------------------------------------
