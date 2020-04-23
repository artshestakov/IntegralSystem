#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISSironaResearchObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSironaResearchObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISSironaResearchObjectForm();

	bool Save() override;

protected:
	void AfterShowEvent() override;
};
//-----------------------------------------------------------------------------
