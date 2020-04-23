#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISListObjectForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISListObjectForm(const QString &TableName, int ParentObjectID, QWidget *parent = 0);
	virtual ~ISListObjectForm();
};
//-----------------------------------------------------------------------------
