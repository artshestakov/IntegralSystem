#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSearchBase.h"
//-----------------------------------------------------------------------------
class ISComboSearchString : public ISComboSearchBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchString(QWidget *parent = 0);
	virtual ~ISComboSearchString();
};
//-----------------------------------------------------------------------------
