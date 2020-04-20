#include "ISOrganizationMyListForm.h"
#include "ISMetaUser.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISProtocol.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
static QString QU_RETURN_ORGANIZATION = PREPARE_QUERY2("UPDATE organizations SET orgz_user = NULL WHERE orgz_id = :ObjectID");
//-----------------------------------------------------------------------------
ISOrganizationMyListForm::ISOrganizationMyListForm(QWidget *parent) : ISListBaseForm("Organizations", parent)
{
	GetQueryModel()->SetClassFilter(QString("orgz_user = %1").arg(CURRENT_USER_ID));
	
	QAction *ActionReturn = new QAction(GetToolBar());
	ActionReturn->setText(LANG("ReturnOrganization"));
	ActionReturn->setToolTip(LANG("ReturnOrganization"));
	ActionReturn->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("ReturnOrganization"));
	connect(ActionReturn, &QAction::triggered, this, &ISOrganizationMyListForm::Return);
	AddAction(ActionReturn, false, true);
}
//-----------------------------------------------------------------------------
ISOrganizationMyListForm::~ISOrganizationMyListForm()
{

}
//-----------------------------------------------------------------------------
void ISOrganizationMyListForm::Return()
{
	ISVectorInt Objects = GetSelectedIDs();
	if (!Objects.empty())
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ReturnOrganization").arg(Objects.size())))
		{
			for (int i = 0; i < Objects.size(); ++i)
			{
				int ObjectID = Objects.at(i);
				QString OrganizationName = GetRecordValue("Name", GetRowIndex(ObjectID)).toString();

				ISQuery qReturn(QU_RETURN_ORGANIZATION);
				qReturn.BindValue(":ObjectID", ObjectID);
				if (qReturn.Execute())
				{
					ISProtocol::Insert(true, "{34276F25-252F-4AF0-859F-D18AB94BF5FF}", GetMetaTable()->Name, GetMetaTable()->LocalListName, GetObjectID(), OrganizationName);
				}
			}

			ISMessageBox::ShowInformation(this, LANG("Message.Information.ReturnOrganizationDone").arg(Objects.size()));
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
