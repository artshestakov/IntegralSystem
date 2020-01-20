#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISDatabaseEdit : public ISComboEdit
{
	Q_OBJECT

public:
	ISDatabaseEdit(QWidget *parent = 0);
	virtual ~ISDatabaseEdit();
};
//-----------------------------------------------------------------------------
