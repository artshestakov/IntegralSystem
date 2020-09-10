#include "ISUserGroupListForm.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISUserGroupForm.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISUserGroupListForm::ISUserGroupListForm(QWidget *parent) : ISListBaseForm("_UserGroup", parent)
{
	QAction *ActionAccess = new QAction(GetToolBar());
	ActionAccess->setText(LANG("AccessRights"));
	ActionAccess->setIcon(BUFFER_ICONS("AccessRights"));
	connect(ActionAccess, &QAction::triggered, this, &ISUserGroupListForm::SelectedGroup);
	AddAction(ActionAccess, true, true);
}
//-----------------------------------------------------------------------------
ISUserGroupListForm::~ISUserGroupListForm()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupListForm::SelectedGroup()
{
	ISGui::SetWaitGlobalCursor(true);
	ISUserGroupForm UserGroupForm(GetObjectID(), GetCurrentRecordValue("Name").toString());
	ISGui::SetWaitGlobalCursor(false);
	UserGroupForm.Exec();
}
//-----------------------------------------------------------------------------
