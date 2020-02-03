#include "ISIncomingCallInformResourceForm.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_ORGANIZATION = PREPARE_QUERY("SELECT orgz_name FROM organizations WHERE orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
ISIncomingCallInformResourceForm::ISIncomingCallInformResourceForm(const QVariantMap &payload, QWidget *parent) : ISIncomingCallBaseForm(payload, parent)
{
	OrganizationID = payload.value("Payload").toInt();

	ISQuery qSelect(QS_ORGANIZATION);
	qSelect.BindValue(":OrganizationID", OrganizationID);
	if (qSelect.ExecuteFirst())
	{
		AddLabel(LANG("CallingOrganization") + ':', qSelect.ReadColumn("orgz_name").toString());
	}
}
//-----------------------------------------------------------------------------
ISIncomingCallInformResourceForm::~ISIncomingCallInformResourceForm()
{

}
//-----------------------------------------------------------------------------
void ISIncomingCallInformResourceForm::OpenCard()
{
	ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "Organizations", OrganizationID)->showMaximized();
	close();
}
//-----------------------------------------------------------------------------
