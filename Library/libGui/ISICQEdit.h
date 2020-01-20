#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISICQEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISICQEdit(QWidget *parent);
	virtual ~ISICQEdit();
};
//-----------------------------------------------------------------------------
