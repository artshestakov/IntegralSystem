#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class ISTaskDescriptionEdit : public ISTextEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskDescriptionEdit(QWidget *parent = 0);
	virtual ~ISTaskDescriptionEdit();
};
//-----------------------------------------------------------------------------
