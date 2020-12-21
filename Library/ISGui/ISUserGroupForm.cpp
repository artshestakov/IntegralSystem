#include "ISUserGroupForm.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISScrollArea.h"
#include "ISMetaSystemsEntity.h"
#include "ISMetaData.h"
#include "ISUserRoleEntity.h"
#include "ISControls.h"
#include "ISProtocol.h"
#include "ISGui.h"
#include "ISTcpQuery.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISUserGroupForm::ISUserGroupForm(int group_id, const QString &group_name)
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
ISUserGroupForm::~ISUserGroupForm()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	ISGui::SetWaitGlobalCursor(true);

	ISTcpQuery qGetGroupRights(API_GET_GROUP_RIGHTS);
	qGetGroupRights.BindValue("GroupID", GroupID);
	if (qGetGroupRights.Execute())
	{
		QVariantMap ResultMap = qGetGroupRights.TakeAnswer();
		SubSystems = ResultMap["SubSystems"].toList();
		Tables = ResultMap["Tables"].toMap();

		CreateSubSystems();
		CreateTables();
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
void ISUserGroupForm::CreateSubSystems()
{
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	TabWidget->addTab(ScrollArea, LANG("AccessRights.SubSystems"));

	for (ISMetaSystem *MetaSystem : ISMetaSystemsEntity::Instance().GetSystems()) //Обход всех систем
	{
		QGroupBox *GroupBox = new QGroupBox(MetaSystem->LocalName, ScrollArea);
		GroupBox->setLayout(new QVBoxLayout());
		GroupBox->setStyleSheet(BUFFER_STYLE_SHEET("QGroupBoxAccessSubSystem"));
		ScrollArea->widget()->layout()->addWidget(GroupBox);

		for (ISMetaSubSystem *SubSystem : MetaSystem->SubSystems) //Обход всех подсистем текущей системы
		{
			ISCheckEdit *CheckEdit = new ISCheckEdit(GroupBox);
			CheckEdit->SetValue(SubSystems.contains(SubSystem->UID));
			CheckEdit->SetText(SubSystem->LocalName);
			CheckEdit->setProperty("SubSystemUID", SubSystem->UID);
			CheckEdit->SetToolTip(LANG("AccessRights.ClickedToGiveAccessFromSubSystem"));
			connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupForm::SubSystemClicked);
			GroupBox->layout()->addWidget(CheckEdit);
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupForm::CreateTables()
{
	QFormLayout *FormLayout = new QFormLayout();
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(FormLayout);
	TabWidget->addTab(ScrollArea, LANG("AccessRights.Tables"));

	//Вытаскиваем типы прав на таблицы
	ISVectorMap AccessTypeVector = ISBuffer::Instance().GetAccessTableType();

	//Вытаскиваем из мета-данных все НЕ системные таблицы
	ISStringMap Map;
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		if (MetaTable->IsSystem) //Если таблица является системной - пропускать
		{
			continue;
		}
		
		QToolBar *ToolBar = new QToolBar(ScrollArea);
		ToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		ToolBar->setIconSize(ISDefines::Gui::SIZE_20_20);
		connect(ToolBar, &QToolBar::actionTriggered, this, &ISUserGroupForm::TableClicked);
		FormLayout->addRow(MetaTable->LocalListName + ':', ToolBar);

		for (const QVariantMap &VariantMap : AccessTypeVector) //Обходим типы прав
		{
			QAction *Action = ToolBar->addAction(BUFFER_ICONS(VariantMap["Icon"].toString()), VariantMap["Name"].toString());
			Action->setCheckable(true);
			Action->setProperty("TableName", MetaTable->Name);
			Action->setProperty("LocalName", MetaTable->LocalListName);
			Action->setProperty("AccessUID", VariantMap["AccessUID"]);
			Action->setChecked(Tables.contains(MetaTable->Name) && Tables.value(MetaTable->Name).toList().contains(VariantMap["AccessUID"]));
			ToolBar->widgetForAction(Action)->setCursor(CURSOR_POINTING_HAND);
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupForm::CreateSpecial(const QVariantList &SpecialRights)
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
			connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupForm::SpecialClicked);
			GroupBox->layout()->addWidget(CheckEdit);
		}
		dynamic_cast<QVBoxLayout*>(GroupBox->layout())->addStretch();
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupForm::SubSystemClicked(const QVariant &value)
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
void ISUserGroupForm::TableClicked(QAction *Action)
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
void ISUserGroupForm::SpecialClicked(const QVariant &value)
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
