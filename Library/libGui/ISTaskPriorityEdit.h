#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISRadioEdit.h"
//-----------------------------------------------------------------------------
class ISTaskPriorityEdit : public ISRadioEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskPriorityEdit(QWidget *parent);
	virtual ~ISTaskPriorityEdit();
};
//-----------------------------------------------------------------------------
