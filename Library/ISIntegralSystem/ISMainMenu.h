#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISMainMenu : public QMenu
{
	Q_OBJECT

public:
	ISMainMenu(QWidget *parent = 0);
	virtual ~ISMainMenu();

	void Show(const QPoint &pos);

private:
	QPropertyAnimation *PropertyAnimation;
};
//-----------------------------------------------------------------------------
