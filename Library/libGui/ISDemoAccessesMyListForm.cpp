#include "ISDemoAccessesMyListForm.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
ISDemoAccessesMyListForm::ISDemoAccessesMyListForm(QWidget *parent) : ISListBaseForm("DemoAccesses", parent)
{
	GetQueryModel()->SetClassFilter("dacc_executor = " + QString::number(ISMetaUser::Instance().UserData->ID));
}
//-----------------------------------------------------------------------------
ISDemoAccessesMyListForm::~ISDemoAccessesMyListForm()
{

}
//-----------------------------------------------------------------------------
