#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISOrganizationScoreObjectForm : public ISObjectFormBase
{
	Q_OBJECT
		
public:
	Q_INVOKABLE ISOrganizationScoreObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISOrganizationScoreObjectForm();

	void SetOrganization(int OrganizationID);

protected:
	bool Save() override;
};
//-----------------------------------------------------------------------------
