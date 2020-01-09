#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPushButton : public QPushButton
{
	Q_OBJECT

public:
	ISPushButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent = 0);
	ISPushButton(const QIcon &Icon, const QString &Text, QWidget *parent = 0);
	ISPushButton(const QString &Text, QWidget *parent = 0);
	ISPushButton(const QIcon &Icon, QWidget *parent = 0);
	ISPushButton(QWidget *parent = 0);
	virtual ~ISPushButton();
};
//-----------------------------------------------------------------------------
