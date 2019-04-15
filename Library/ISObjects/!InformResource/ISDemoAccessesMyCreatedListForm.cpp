#include "StdAfx.h"
#include "ISDemoAccessesMyCreatedListForm.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
ISDemoAccessesMyCreatedListForm::ISDemoAccessesMyCreatedListForm(QWidget *parent) : ISListBaseForm("DemoAccesses", parent)
{
	GetQueryModel()->SetClassFilter("dacc_registrationuser = " + QString::number(CURRENT_USER_ID));
}
//-----------------------------------------------------------------------------
ISDemoAccessesMyCreatedListForm::~ISDemoAccessesMyCreatedListForm()
{

}
//-----------------------------------------------------------------------------
