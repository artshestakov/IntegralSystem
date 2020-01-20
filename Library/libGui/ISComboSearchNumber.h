#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSearchBase.h"
//-----------------------------------------------------------------------------
class ISComboSearchNumber : public ISComboSearchBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchNumber(QWidget *parent = 0);
	virtual ~ISComboSearchNumber();
};
//-----------------------------------------------------------------------------
