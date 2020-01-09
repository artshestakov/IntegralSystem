#include "StdAfx.h"
#include "ISHighwayOrganizationListForm.h"
//-----------------------------------------------------------------------------
ISHighwayOrganizationListForm::ISHighwayOrganizationListForm(QWidget *parent) : ISListBaseForm("Organization", parent)
{
	GetQueryModel()->SetClassFilter("orgz.orgz_parent IS NULL");
}
//-----------------------------------------------------------------------------
ISHighwayOrganizationListForm::~ISHighwayOrganizationListForm()
{

}
//-----------------------------------------------------------------------------
