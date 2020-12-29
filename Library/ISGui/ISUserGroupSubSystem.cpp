#include "ISUserGroupSubSystem.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISUserGroupForm.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISUserGroupSubSystem::ISUserGroupSubSystem(QWidget *parent) : ISListBaseForm("_UserGroup", parent)
{
	QAction *ActionAccess = new QAction(GetToolBar());
	ActionAccess->setText(LANG("AccessRights"));
	ActionAccess->setIcon(BUFFER_ICONS("AccessRights"));
	connect(ActionAccess, &QAction::triggered, this, &ISUserGroupSubSystem::SelectedGroup);
	AddAction(ActionAccess, true, true);
}
//-----------------------------------------------------------------------------
ISUserGroupSubSystem::~ISUserGroupSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupSubSystem::SelectedGroup()
{
	ISGui::SetWaitGlobalCursor(true);
	ISUserGroupForm UserGroupForm(GetObjectID(), GetCurrentRecordValue("Name").toString());
	ISGui::SetWaitGlobalCursor(false);
	UserGroupForm.Exec();
}
//-----------------------------------------------------------------------------