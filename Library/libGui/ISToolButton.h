#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISToolButton : public QToolButton
{
	Q_OBJECT

public:
	ISToolButton(QWidget *parent = 0);
	virtual ~ISToolButton();
};
//-----------------------------------------------------------------------------
