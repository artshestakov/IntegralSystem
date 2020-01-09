#include "StdAfx.h"
#include "ISDemoAccessesMyListForm.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
ISDemoAccessesMyListForm::ISDemoAccessesMyListForm(QWidget *parent) : ISListBaseForm("DemoAccesses", parent)
{
	GetQueryModel()->SetClassFilter("dacc_executor = " + QString::number(CURRENT_USER_ID));
}
//-----------------------------------------------------------------------------
ISDemoAccessesMyListForm::~ISDemoAccessesMyListForm()
{

}
//-----------------------------------------------------------------------------
