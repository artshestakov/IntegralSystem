#include "ISHighwayIncomingCallForm.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_ORGANIZATION = PREPARE_QUERY2("SELECT orgz_name FROM organization WHERE orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
ISHighwayIncomingCallForm::ISHighwayIncomingCallForm(const QVariantMap &payload) : ISIncomingCallBaseForm(payload)
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
ISHighwayIncomingCallForm::~ISHighwayIncomingCallForm()
{

}
//-----------------------------------------------------------------------------
void ISHighwayIncomingCallForm::OpenCard()
{
	ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "Organization", OrganizationID)->showMaximized();
	close();
}
//-----------------------------------------------------------------------------
