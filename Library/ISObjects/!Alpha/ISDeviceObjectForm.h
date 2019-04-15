#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectFormBase.h"
#include "ISListEdit.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISDeviceObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDeviceObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
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
