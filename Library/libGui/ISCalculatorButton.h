#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISCalculatorButton : public ISPushButton
{
    Q_OBJECT

public:
    ISCalculatorButton(QWidget *parent = 0);
	virtual ~ISCalculatorButton();
};
//-----------------------------------------------------------------------------
