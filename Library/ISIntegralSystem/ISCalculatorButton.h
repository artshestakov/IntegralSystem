#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISCalculatorButton : public ISPushButton
{
    Q_OBJECT

public:
    ISCalculatorButton(QWidget *parent = 0);
	virtual ~ISCalculatorButton();
};
//-----------------------------------------------------------------------------
