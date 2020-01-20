#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISDemoAccessesMyCreatedListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDemoAccessesMyCreatedListForm(QWidget *parent = 0);
	virtual ~ISDemoAccessesMyCreatedListForm();
};
//-----------------------------------------------------------------------------
