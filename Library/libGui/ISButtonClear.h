#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISButtonClear : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClear(QWidget *parent = 0);
	virtual ~ISButtonClear();
};
//-----------------------------------------------------------------------------
