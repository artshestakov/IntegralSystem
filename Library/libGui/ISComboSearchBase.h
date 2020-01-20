#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboEdit.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISComboSearchBase : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchBase(QWidget *parent = 0);
	virtual ~ISComboSearchBase();

	ISNamespace::SearchOperatorType GetOperator();
};
//-----------------------------------------------------------------------------
