#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISOKPOEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOKPOEdit(QWidget *parent);
	virtual ~ISOKPOEdit();
};
//-----------------------------------------------------------------------------
