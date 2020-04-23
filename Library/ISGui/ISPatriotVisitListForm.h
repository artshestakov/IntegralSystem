#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISPatriotVisitListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotVisitListForm(QWidget *parent = 0);
	virtual ~ISPatriotVisitListForm();

protected:
	void CancelVisit();
};
//-----------------------------------------------------------------------------
