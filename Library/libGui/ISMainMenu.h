#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISMainMenu : public QMenu
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
