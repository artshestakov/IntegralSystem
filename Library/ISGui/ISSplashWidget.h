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

	void SetText(const QString &Text);

protected:
	void showEvent(QShowEvent *e);

private:
	QLabel *LabelText;
};
//-----------------------------------------------------------------------------
