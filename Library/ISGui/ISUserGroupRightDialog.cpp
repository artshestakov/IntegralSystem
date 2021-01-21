#include "ISUserGroupRightDialog.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISScrollArea.h"
#include "ISMetaSystemsEntity.h"
#include "ISUserRoleEntity.h"
#include "ISControls.h"
#include "ISGui.h"
#include "ISTcpQuery.h"
#include "ISDialogsCommon.h"
//-----------------------------------------------------------------------------
ISUserGroupRightDialog::ISUserGroupRightDialog(int group_id, const QString &group_name)
	: ISInterfaceDialogForm(),
	GroupID(group_id)
{
	setMinimumSize(800, 750);
	setWindowTitle(LANG("AccessRights.SettingAcceessFromGroup").arg(group_name));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	TabWidget = new QTabWidget(this);
	GetMainLayout()->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
ISUserGroupRightDialog::~ISUserGroupRightDialog()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	ISGui::SetWaitGlobalCursor(true);

	ISTcpQuery qGetGroupRights(API_GET_GROUP_RIGHTS);
	qGetGroupRights.BindValue("GroupID", GroupID);
	if (qGetGroupRights.Execute())
	{
		QVariantMap ResultMap = qGetGroupRights.TakeAnswer();
		CreateSubSystems(ResultMap["Systems"].toList());
		CreateTables(ResultMap["RightsTableType"].toList(), ResultMap["Tables"].toList());
		CreateSpecial(ResultMap["Special"].toList());
		ISGui::SetWaitGlobalCursor(false);
	}
	else
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, qGetGroupRights.GetErrorString());
		close();
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::CreateSubSystems(const QVariantList &Systems)
{
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	TabWidget->addTab(ScrollArea, LANG("AccessRights.SubSystems"));

	for (const QVariant &VariantSystem : Systems) //Обход всех систем
	{
		QVariantMap SystemMap = VariantSystem.toMap();

		QGroupBox *GroupBox = new QGroupBox(SystemMap["LocalName"].toString(), ScrollArea);
		GroupBox->setLayout(new QVBoxLayout());
		GroupBox->setStyleSheet(BUFFER_STYLE_SHEET("QGroupBoxAccessSubSystem"));
		ScrollArea->widget()->layout()->addWidget(GroupBox);

		for (const QVariant &VariantSubSystem : SystemMap["SubSystems"].toList()) //Обход всех подсистем текущей системы
		{
			QVariantMap SubSystemMap = VariantSubSystem.toMap();

			ISCheckEdit *CheckEdit = new ISCheckEdit(GroupBox);
			CheckEdit->SetValue(SubSystemMap["IsExist"]);
			CheckEdit->SetText(SubSystemMap["LocalName"].toString());
			CheckEdit->setProperty("SubSystemUID", SubSystemMap["UID"]);
			CheckEdit->SetToolTip(LANG("AccessRights.ClickedToGiveAccessFromSubSystem"));
			connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupRightDialog::SubSystemClicked);
			GroupBox->layout()->addWidget(CheckEdit);
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::CreateTables(const QVariantList &RightTableType, const QVariantList &Tables)
{
	QFormLayout *FormLayout = new QFormLayout();
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(FormLayout);
	TabWidget->addTab(ScrollArea, LANG("AccessRights.Tables"));

	for (const QVariant &VariantTable : Tables)
	{
		QVariantMap TableMap = VariantTable.toMap();

		QToolBar *ToolBar = new QToolBar(ScrollArea);
		ToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		ToolBar->setIconSize(ISDefines::Gui::SIZE_20_20);
		connect(ToolBar, &QToolBar::actionTriggered, this, &ISUserGroupRightDialog::TableClicked);
		FormLayout->addRow(TableMap["LocalName"].toString() + ':', ToolBar);

		for (const QVariant &VariantRight : RightTableType) //Обходим типы прав
		{
			QVariantMap RightMap = VariantRight.toMap();

			QAction *Action = ToolBar->addAction(BUFFER_ICONS(RightMap["Icon"].toString()), RightMap["Name"].toString());
			Action->setCheckable(true);
			Action->setProperty("TableName", TableMap["TableName"]);
			Action->setProperty("AccessUID", RightMap["UID"]);
			Action->setChecked(TableMap["Rights"].toList().contains(RightMap["UID"]));
			ToolBar->widgetForAction(Action)->setCursor(CURSOR_POINTING_HAND);
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::CreateSpecial(const QVariantList &SpecialRights)
{
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	TabWidget->addTab(ScrollArea, LANG("AccessRights.SpecialRoles"));

	//Обходим группы спец. прав
	for (const QVariant &SpecialRightGroup : SpecialRights)
	{
		QVariantMap SpecialRightGroupMap = SpecialRightGroup.toMap();

		QGroupBox *GroupBox = new QGroupBox(SpecialRightGroupMap["LocalName"].toString() + ':', ScrollArea);
		GroupBox->setLayout(new QVBoxLayout());
		GroupBox->setStyleSheet(BUFFER_STYLE_SHEET("QGroupBoxAccessSubSystem"));
		ScrollArea->widget()->layout()->addWidget(GroupBox);

		//Обходим спец. права текущей группы
		for (const QVariant &SpecialRight : SpecialRightGroupMap["Rights"].toList())
		{
			QVariantMap SpecialRightMap = SpecialRight.toMap();

			ISCheckEdit *CheckEdit = new ISCheckEdit(GroupBox);
			CheckEdit->SetValue(SpecialRightMap["IsExist"]);
			CheckEdit->SetText(SpecialRightMap["LocalName"].toString());
			CheckEdit->CreateHint(SpecialRightMap["Hint"].toString());
			CheckEdit->setProperty("UID", SpecialRightMap["UID"]);
			connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupRightDialog::SpecialClicked);
			GroupBox->layout()->addWidget(CheckEdit);
		}
		dynamic_cast<QVBoxLayout*>(GroupBox->layout())->addStretch();
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::SubSystemClicked(const QVariant &value)
{
	ISGui::SetWaitGlobalCursor(true);
	ISTcpQuery qAlterAccess(value.toBool() ? API_GROUP_RIGHT_SUBSYSTEM_ADD : API_GROUP_RIGHT_SUBSYSTEM_DELETE);
	qAlterAccess.BindValue("GroupID", GroupID);
	qAlterAccess.BindValue("UID", sender()->property("SubSystemUID"));
	if (!qAlterAccess.Execute())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, qAlterAccess.GetErrorString());
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::TableClicked(QAction *Action)
{
	ISGui::SetWaitGlobalCursor(true);
	ISTcpQuery qAlterAccess(Action->isChecked() ? API_GROUP_RIGHT_TABLE_ADD : API_GROUP_RIGHT_TABLE_DELETE);
	qAlterAccess.BindValue("GroupID", GroupID);
	qAlterAccess.BindValue("TableName", Action->property("TableName"));
	qAlterAccess.BindValue("AccessUID", Action->property("AccessUID"));
	if (!qAlterAccess.Execute())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, qAlterAccess.GetErrorString());
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::SpecialClicked(const QVariant &value)
{
	ISGui::SetWaitGlobalCursor(true);
	ISTcpQuery qGroupRightSpecial(value.toBool() ? API_GROUP_RIGHT_SPECIAL_ADD : API_GROUP_RIGHT_SPECIAL_DELETE);
	qGroupRightSpecial.BindValue("GroupID", GroupID);
	qGroupRightSpecial.BindValue("SpecialRightUID", sender()->property("UID"));
	if (!qGroupRightSpecial.Execute())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, qGroupRightSpecial.GetErrorString());
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
