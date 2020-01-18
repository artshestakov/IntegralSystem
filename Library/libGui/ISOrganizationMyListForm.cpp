#include "StdAfx.h"
#include "ISOrganizationMyListForm.h"
#include "ISMetaUser.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISProtocol.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
static QString QU_RETURN_ORGANIZATION = PREPARE_QUERY("UPDATE organizations SET orgz_user = NULL WHERE orgz_id = :ObjectID");
//-----------------------------------------------------------------------------
ISOrganizationMyListForm::ISOrganizationMyListForm(QWidget *parent) : ISListBaseForm("Organizations", parent)
{
	GetQueryModel()->SetClassFilter(QString("orgz_user = %1").arg(ISMetaUser::GetInstance().GetData()->ID));
	
	QAction *ActionReturn = new QAction(GetToolBar());
	ActionReturn->setText(LOCALIZATION("ReturnOrganization"));
	ActionReturn->setToolTip(LOCALIZATION("ReturnOrganization"));
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
	QVectorInt Objects = GetSelectedIDs();
	if (Objects.count())
	{
		if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.ReturnOrganization").arg(Objects.count())))
		{
			for (int i = 0; i < Objects.count(); i++)
			{
				int ObjectID = Objects.at(i);
				QString OrganizationName = GetRecordValue("Name", GetRowIndex(ObjectID)).toString();

				ISQuery qReturn(QU_RETURN_ORGANIZATION);
				qReturn.BindValue(":ObjectID", ObjectID);
				if (qReturn.Execute())
				{
					ISProtocol::Insert(true, "{34276F25-252F-4AF0-859F-D18AB94BF5FF}", GetMetaTable()->GetName(), GetMetaTable()->GetLocalListName(), GetObjectID(), OrganizationName);
				}
			}

			ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.ReturnOrganizationDone").arg(Objects.count()));
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
