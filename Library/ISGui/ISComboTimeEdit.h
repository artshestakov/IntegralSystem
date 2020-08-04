#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISComboTimeEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTimeEdit(QWidget *parent = 0);
	virtual ~ISComboTimeEdit();
};
//-----------------------------------------------------------------------------
