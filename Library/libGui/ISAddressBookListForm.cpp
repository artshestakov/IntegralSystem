#include "StdAfx.h"
#include "ISAddressBookListForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISAddressBookListForm::ISAddressBookListForm(QWidget *parent) : ISListBaseForm("_AddressBook", parent)
{
	setWindowTitle(LANG("AddressBook"));
	setWindowIcon(BUFFER_ICONS("AddressBook"));
	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_5_PX);
	connect(this, &ISAddressBookListForm::AddFormFromTab, this, &ISAddressBookListForm::CreateObjectForm);
	GetQueryModel()->SetClassFilter("abok_user = currentuserid()");
}
//-----------------------------------------------------------------------------
ISAddressBookListForm::~ISAddressBookListForm()
{

}
//-----------------------------------------------------------------------------
void ISAddressBookListForm::CreateObjectForm(QWidget *ObjectFormBase)
{
	ObjectFormBase->setParent(nullptr);
	ObjectFormBase->showMaximized();
}
//-----------------------------------------------------------------------------
