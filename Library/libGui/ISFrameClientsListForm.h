#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISFrameClientsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFrameClientsListForm(QWidget *parent);
	virtual ~ISFrameClientsListForm();
};
//-----------------------------------------------------------------------------
