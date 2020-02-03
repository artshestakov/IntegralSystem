#include "ISSironaResearchTypeObjectForm.h"
#include "ISDoubleEdit.h"
//-----------------------------------------------------------------------------
ISSironaResearchTypeObjectForm::ISSironaResearchTypeObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Bonus"), &ISFieldEditBase::ValueChange, this, &ISSironaResearchTypeObjectForm::BonusChanged);
}
//-----------------------------------------------------------------------------
ISSironaResearchTypeObjectForm::~ISSironaResearchTypeObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISSironaResearchTypeObjectForm::BonusChanged(const QVariant &value)
{
	if (value.isValid())
	{
		double Bonus = value.toDouble();
		double CenterSeven = Bonus / 2;
		GetFieldWidget("CenterSeven")->SetValue(CenterSeven);
	}
	else
	{
		GetFieldWidget("CenterSeven")->Clear();
	}
}
//-----------------------------------------------------------------------------
