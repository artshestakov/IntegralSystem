#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISMainMenu : public QMenu
{
	Q_OBJECT

public:
	ISMainMenu(QWidget *parent = 0);
	virtual ~ISMainMenu();

	void Show(const QPoint &Point);

private:
	QPropertyAnimation *PropertyAnimation;
};
//-----------------------------------------------------------------------------
