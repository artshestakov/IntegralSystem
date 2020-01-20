#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISDivisionCodeEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDivisionCodeEdit(QWidget *parent);
	virtual ~ISDivisionCodeEdit();

	bool IsValid() const;
};
//-----------------------------------------------------------------------------
