#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCalculatorButton : public ISPushButton
{
    Q_OBJECT

public:
    ISCalculatorButton(QWidget *parent = 0);
	virtual ~ISCalculatorButton();
};
//-----------------------------------------------------------------------------
