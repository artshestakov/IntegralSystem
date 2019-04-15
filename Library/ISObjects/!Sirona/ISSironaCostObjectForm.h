#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISSironaCostObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSironaCostObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISSironaCostObjectForm();

protected:
	void AfterShowEvent() override;
	bool Save() override;
};
//-----------------------------------------------------------------------------
