#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISIPAddressEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIPAddressEdit(QWidget *parent);
	virtual ~ISIPAddressEdit();
};
//-----------------------------------------------------------------------------
