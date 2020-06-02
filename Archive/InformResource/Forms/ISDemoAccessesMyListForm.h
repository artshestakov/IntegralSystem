#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISDemoAccessesMyListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDemoAccessesMyListForm(QWidget *parent = 0);
	virtual ~ISDemoAccessesMyListForm();
};
//-----------------------------------------------------------------------------
