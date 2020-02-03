#include "ISOrganizationScoreObjectForm.h"
#include "ISNotifySender.h"
//-----------------------------------------------------------------------------
ISOrganizationScoreObjectForm::ISOrganizationScoreObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISOrganizationScoreObjectForm::~ISOrganizationScoreObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOrganizationScoreObjectForm::SetOrganization(int OrganizationID)
{
	GetFieldWidget("Organization")->SetValue(OrganizationID);
}
//-----------------------------------------------------------------------------
bool ISOrganizationScoreObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result)
	{
		ISNotifySender::GetInstance().SendToUser("{B40F9074-D3CB-4006-AA51-D48BD45825DE}", GetFieldValue("Executor").toInt());
	}

	return Result;
}
//-----------------------------------------------------------------------------
