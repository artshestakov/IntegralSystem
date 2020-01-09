#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISButtonClose : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClose(QWidget *parent = 0);
	virtual ~ISButtonClose();
};
//-----------------------------------------------------------------------------
