#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISTaskAllListForm : public ISTaskBaseListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskAllListForm(QWidget *parent = 0);
	virtual ~ISTaskAllListForm();
};
//-----------------------------------------------------------------------------
