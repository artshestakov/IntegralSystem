#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISBIKEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBIKEdit(QWidget *parent);
	virtual ~ISBIKEdit();
};
//-----------------------------------------------------------------------------
