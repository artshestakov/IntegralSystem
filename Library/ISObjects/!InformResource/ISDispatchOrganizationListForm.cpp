#include "StdAfx.h"
#include "ISDispatchOrganizationListForm.h"
#include "ISControls.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_ORGANIZATION = PREPARE_QUERY("SELECT COUNT(*) FROM dispatchorganizations WHERE dorg_dispatch = :DispatchID AND dorg_organization = :OrganizationID");
//-----------------------------------------------------------------------------
static QString QI_ORGANIZATION = PREPARE_QUERY("INSERT INTO dispatchorganizations(dorg_dispatch, dorg_organization) "
											   "VALUES(:DispatchID, :OrganizationID)");
//-----------------------------------------------------------------------------
ISDispatchOrganizationListForm::ISDispatchOrganizationListForm(QWidget *parent) : ISListBaseForm("DispatchOrganizations", parent)
{
	
}
//-----------------------------------------------------------------------------
ISDispatchOrganizationListForm::~ISDispatchOrganizationListForm()
{

}
//-----------------------------------------------------------------------------
void ISDispatchOrganizationListForm::LoadData()
{
	GetQueryModel()->SetClassFilter(QString("dorg_dispatch = %1").arg(GetParentObjectID()));
	ISListBaseForm::LoadData();
}
//-----------------------------------------------------------------------------
void ISDispatchOrganizationListForm::Picking()
{
	
}
//-----------------------------------------------------------------------------
void ISDispatchOrganizationListForm::Matched(const QVectorInt &Vector)
{
	for (int OrganizationID : Vector)
	{
		ISQuery qSelect(QS_ORGANIZATION);
		qSelect.BindValue(":DispatchID", GetParentObjectID());
		qSelect.BindValue(":OrganizationID", OrganizationID);
		if (qSelect.ExecuteFirst())
		{
			int Count = qSelect.ReadColumn("count").toInt();
			if (!Count)
			{
				ISQuery qInsert(QI_ORGANIZATION);
				qInsert.BindValue(":DispatchID", GetParentObjectID());
				qInsert.BindValue(":OrganizationID", OrganizationID);
				qInsert.Execute();
			}
		}
	}

	if (Vector.count())
	{
		Update();
	}
}
//-----------------------------------------------------------------------------
