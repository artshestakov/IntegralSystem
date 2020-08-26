#include "ISUserGroupListForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISUserGroupListForm::ISUserGroupListForm(QWidget *parent) : ISListBaseForm("_UserGroup", parent)
{
	Label = new QLabel(this);
	Label->setWordWrap(true);
	Label->setText(LANG("SelectedUserGroupLeftFromAccessSetting"));
	Label->setFont(ISDefines::Gui::FONT_TAHOMA_12);
	Label->setSizePolicy(QSizePolicy::Minimum, Label->sizePolicy().verticalPolicy());
	GetLayoutTableView()->addWidget(Label, 0, Qt::AlignCenter);

	UserGroupWidget = nullptr;
	connect(this, &ISUserGroupListForm::SelectedRowSignal, this, &ISUserGroupListForm::SelectedGroup);
}
//-----------------------------------------------------------------------------
ISUserGroupListForm::~ISUserGroupListForm()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupListForm::SelectedGroup()
{
	POINTER_DELETE(UserGroupWidget);
	if (GetCountSelected())
	{
		Label->setVisible(false);

		ISGui::SetWaitGlobalCursor(true);
		UserGroupWidget = new ISUserGroupWidget(GetObjectID(), GetCurrentRecordValue("Name").toString(), this);
		GetLayoutTableView()->addWidget(UserGroupWidget);
		ISGui::SetWaitGlobalCursor(false);
	}
	else
	{
		Label->setVisible(true);
	}
}
//-----------------------------------------------------------------------------
