#include "ISDemoAccessesObjectForm.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
static QString QU_RESULT_DATE = PREPARE_QUERY("UPDATE demoaccesses SET dacc_resultdatetime = now() WHERE dacc_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QU_ORGANIZATION_USER = PREPARE_QUERY("UPDATE organizations SET orgz_user = :Executor WHERE orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
static QString QU_EDIT_USER = PREPARE_QUERY("UPDATE demoaccesses SET dacc_edituser = :EditUserID WHERE dacc_id = :ObjectID");
//-----------------------------------------------------------------------------
ISDemoAccessesObjectForm::ISDemoAccessesObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISDemoAccessesObjectForm::~ISDemoAccessesObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISDemoAccessesObjectForm::SetOrganizationID(int OrganizationID)
{
	GetFieldWidget("Organization")->SetValue(OrganizationID);
}
//-----------------------------------------------------------------------------
void ISDemoAccessesObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();
	
	Result = GetFieldValue("Result");
}
//-----------------------------------------------------------------------------
bool ISDemoAccessesObjectForm::Save()
{
	bool Saved = ISObjectFormBase::Save();
	if (Saved)
	{
		if (Result != GetFieldValue("Result"))
		{
			ISQuery qUpdate(QU_RESULT_DATE);
			qUpdate.BindValue(":ObjectID", GetObjectID());
			qUpdate.Execute();
		}

		ISQuery qUpdate(QU_ORGANIZATION_USER);
		qUpdate.BindValue(":Executor", GetFieldValue("Executor"));
		qUpdate.BindValue(":OrganizationID", GetFieldValue("Organization"));
		qUpdate.Execute();

		ISQuery qEditUser(QU_EDIT_USER);
		qEditUser.BindValue(":EditUserID", ISMetaUser::GetInstance().GetData()->ID);
		qEditUser.BindValue(":ObjectID", GetObjectID());
		qEditUser.Execute();
	}

	return Saved;
}
//-----------------------------------------------------------------------------
