#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISDoubleEdit.h"
//-----------------------------------------------------------------------------
class ISPercentEdit : public ISDoubleEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPercentEdit(QWidget *parent);
	virtual ~ISPercentEdit();
};
//-----------------------------------------------------------------------------
