#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
#include "ISListEdit.h"
//-----------------------------------------------------------------------------
class ISDeviceObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDeviceObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISDeviceObjectForm();

protected:
	void AfterShowEvent() override;
	void ClearModel();
	void BrandChanged(const QVariant &Value);

private:
	ISListEdit *ListEditBrand;
	ISListEdit *ListEditModel;
};
//-----------------------------------------------------------------------------
