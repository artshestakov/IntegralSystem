#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISDispatchOrganizationListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDispatchOrganizationListForm(QWidget *parent = 0);
	virtual ~ISDispatchOrganizationListForm();

	void LoadData() override;

protected:
	void Picking();
	void Matched(const ISVectorInt &Vector);
};
//-----------------------------------------------------------------------------
