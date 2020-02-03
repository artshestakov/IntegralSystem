#include "ISHighwayOrganizationMyListForm.h"
//-----------------------------------------------------------------------------
ISHighwayOrganizationMyListForm::ISHighwayOrganizationMyListForm(QWidget *parent) : ISListBaseForm("Organization", parent)
{
	GetQueryModel()->SetClassFilter("orgz_user = currentuserid()");
}
//-----------------------------------------------------------------------------
ISHighwayOrganizationMyListForm::~ISHighwayOrganizationMyListForm()
{

}
//-----------------------------------------------------------------------------
