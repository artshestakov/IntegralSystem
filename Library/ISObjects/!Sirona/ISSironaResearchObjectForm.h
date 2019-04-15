#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISSironaResearchObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSironaResearchObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISSironaResearchObjectForm();

	bool Save() override;

protected:
	void AfterShowEvent() override;
};
//-----------------------------------------------------------------------------
