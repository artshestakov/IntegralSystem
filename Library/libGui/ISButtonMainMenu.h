#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISButtonMainMenu : public QToolButton
{
	Q_OBJECT

public:
	ISButtonMainMenu(QWidget *parent = 0);
	virtual ~ISButtonMainMenu();

protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);

private:
	QIcon IconDefault;
	QIcon IconActive;
};
//-----------------------------------------------------------------------------
