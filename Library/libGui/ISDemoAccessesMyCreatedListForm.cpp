#include "ISDemoAccessesMyCreatedListForm.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
ISDemoAccessesMyCreatedListForm::ISDemoAccessesMyCreatedListForm(QWidget *parent) : ISListBaseForm("DemoAccesses", parent)
{
	GetQueryModel()->SetClassFilter("dacc_registrationuser = " + QString::number(ISMetaUser::Instance().UserData->ID));
}
//-----------------------------------------------------------------------------
ISDemoAccessesMyCreatedListForm::~ISDemoAccessesMyCreatedListForm()
{

}
//-----------------------------------------------------------------------------
