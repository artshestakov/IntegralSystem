#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISServiceButton : public ISPushButton
{
	Q_OBJECT

public:
	ISServiceButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent = 0);
	ISServiceButton(const QIcon &Icon, const QString &Text, QWidget *parent = 0);
	ISServiceButton(const QString &Text, QWidget *parent = 0);
	ISServiceButton(QWidget *parent = 0);
	virtual ~ISServiceButton();
};
//-----------------------------------------------------------------------------
