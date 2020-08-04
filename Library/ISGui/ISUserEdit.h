#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISUserEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserEdit(QWidget *parent = 0);
	virtual ~ISUserEdit();
};
//-----------------------------------------------------------------------------
