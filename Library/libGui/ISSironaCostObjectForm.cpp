#include "ISSironaCostObjectForm.h"
//-----------------------------------------------------------------------------
ISSironaCostObjectForm::ISSironaCostObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISSironaCostObjectForm::~ISSironaCostObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISSironaCostObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();

	if (GetFormType() == ISNamespace::OFT_Edit)
	{
		GetFieldWidget("Sum")->SetReadOnly(true);
	}
}
//-----------------------------------------------------------------------------
bool ISSironaCostObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result && GetFormType() != ISNamespace::OFT_Edit)
	{
		GetFieldWidget("Sum")->SetReadOnly(true);
	}

	return Result;
}
//-----------------------------------------------------------------------------
