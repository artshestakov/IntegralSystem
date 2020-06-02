#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISDispatchListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDispatchListForm(QWidget *parent = 0);
	virtual ~ISDispatchListForm();

private:
	void Send();
};
//-----------------------------------------------------------------------------
