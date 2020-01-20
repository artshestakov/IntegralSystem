#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISCalculatorButton : public ISPushButton
{
    Q_OBJECT

public:
    ISCalculatorButton(QWidget *parent = 0);
	virtual ~ISCalculatorButton();
};
//-----------------------------------------------------------------------------
