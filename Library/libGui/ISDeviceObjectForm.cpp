#include "ISDeviceObjectForm.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISDeviceObjectForm::ISDeviceObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	ISFieldEditBase *ControlBrand = GetFieldWidget("Brand");
	connect(ControlBrand, &ISFieldEditBase::ValueChange, this, &ISDeviceObjectForm::BrandChanged);

	ISFieldEditBase *ControlModel = GetFieldWidget("Model");
	
	ListEditBrand = dynamic_cast<ISListEdit*>(ControlBrand);
	ListEditModel = dynamic_cast<ISListEdit*>(ControlModel);
}
//-----------------------------------------------------------------------------
ISDeviceObjectForm::~ISDeviceObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISDeviceObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();

	QVariant ValueBrand = ListEditBrand->GetValue();
	if (ValueBrand.isValid())
	{
		BrandChanged(ValueBrand);
		ListEditModel->SetEnabled(true);
	}
	else
	{
		ListEditModel->SetEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISDeviceObjectForm::ClearModel()
{
	ListEditModel->ClearSqlFilter();
	ListEditModel->Clear();
	ListEditModel->SetEnabled(false);
}
//-----------------------------------------------------------------------------
void ISDeviceObjectForm::BrandChanged(const QVariant &Value)
{
	if (Value.toString().length())
	{
		int BrandID = Value.toInt();
		if (BrandID)
		{
			QString ModelFilter = "mdls_brand = " + QString::number(BrandID);
			ListEditModel->SetSqlFilter(ModelFilter);
			ListEditModel->SetEnabled(true);
		}
		else
		{
			ClearModel();
		}
	}
	else
	{
		ClearModel();
	}
}
//-----------------------------------------------------------------------------
