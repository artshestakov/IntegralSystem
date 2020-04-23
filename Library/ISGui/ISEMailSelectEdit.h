#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISEMailSelectEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISEMailSelectEdit(QWidget *parent);
	virtual ~ISEMailSelectEdit();
};
//-----------------------------------------------------------------------------
