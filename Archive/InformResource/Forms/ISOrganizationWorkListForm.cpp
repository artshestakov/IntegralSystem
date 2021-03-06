#include "ISOrganizationWorkListForm.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISProtocol.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
static QString QU_ORGANIZATION_USER = PREPARE_QUERY2("UPDATE organizations SET orgz_user = currentuserid() WHERE orgz_id = :ObjectID");
//-----------------------------------------------------------------------------
ISOrganizationWorkListForm::ISOrganizationWorkListForm(QWidget *parent) : ISListBaseForm("Organizations", parent)
{
	GetQueryModel()->SetClassFilter("orgz_perspective");

	QAction *ActionTakeToWork = new QAction(GetToolBar());
	ActionTakeToWork->setText(LANG("TakeOrganization"));
	ActionTakeToWork->setToolTip(LANG("TakeOrganization"));
	ActionTakeToWork->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("TakeOrganization"));
	connect(ActionTakeToWork, &QAction::triggered, this, &ISOrganizationWorkListForm::TakeToWork);
	AddAction(ActionTakeToWork, false, true);
}
//-----------------------------------------------------------------------------
ISOrganizationWorkListForm::~ISOrganizationWorkListForm()
{

}
//-----------------------------------------------------------------------------
void ISOrganizationWorkListForm::TakeToWork()
{
	ISVectorInt Objects = GetSelectedIDs();
	if (!Objects.empty())
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.OrganizationTakeToWork").arg(Objects.size())))
		{
			for (int i = 0; i < Objects.size(); ++i)
			{
				int ObjectID = Objects.at(i);
				QString OrganizationName = GetRecordValue("Name", GetRowIndex(ObjectID)).toString();

				ISQuery qTake(QU_ORGANIZATION_USER);
				qTake.BindValue(":ObjectID", ObjectID);
				if (qTake.Execute())
				{
					ISProtocol::Insert(true, "{075934CA-4E4F-47E6-966F-C3D4991F2B03}", GetMetaTable()->Name, GetMetaTable()->LocalListName, GetObjectID(), OrganizationName);
				}
			}

			ISMessageBox::ShowInformation(this, LANG("Message.Information.OrganizationTakeToWorkDone").arg(Objects.size()));
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
