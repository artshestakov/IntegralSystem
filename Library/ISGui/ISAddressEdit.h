#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISAddressEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAddressEdit(QWidget *parent = 0);
	virtual ~ISAddressEdit();

protected:
	void ListFIAS();
};
//-----------------------------------------------------------------------------
