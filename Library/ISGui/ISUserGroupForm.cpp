#include "ISUserGroupForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISScrollArea.h"
#include "ISMetaSystemsEntity.h"
#include "ISQuery.h"
#include "ISToolBarAccessTable.h"
#include "ISMetaData.h"
#include "ISUserRoleEntity.h"
#include "ISControls.h"
#include "ISProtocol.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL_GROUP = PREPARE_QUERY("SELECT gast_uid, gast_name "
															 "FROM _groupaccessspecialtype "
															 "WHERE gast_parent IS NULL "
															 "ORDER BY gast_order");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL_CHECK = PREPARE_QUERY("SELECT gast_uid, gast_name, gast_hint "
													   "FROM _groupaccessspecialtype "
													   "WHERE gast_parent = :ParentUID "
													   "ORDER BY gast_order");
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

	CreateSubSystems();
	CreateTables();
	CreateSpecial();
}
//-----------------------------------------------------------------------------
ISUserGroupForm::~ISUserGroupForm()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupForm::CreateSubSystems()
{
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	TabWidget->addTab(ScrollArea, LANG("AccessRights.SubSystems"));

	for (ISMetaSystem *MetaSystem : ISMetaSystemsEntity::Instance().GetSystems()) //Обход всех систем
	{
		QFormLayout *FormLayout = new QFormLayout();

		QGroupBox *GroupBox = new QGroupBox(ScrollArea);
		GroupBox->setTitle(MetaSystem->LocalName);
		GroupBox->setLayout(FormLayout);
		GroupBox->setStyleSheet(BUFFER_STYLE_SHEET("QGroupBoxAccessSubSystem"));
		ScrollArea->widget()->layout()->addWidget(GroupBox);

		for (ISMetaSubSystem *SubSystem : MetaSystem->SubSystems) //Обход всех подсистем текущей системы
		{
			ISCheckEdit *CheckEdit = new ISCheckEdit(GroupBox);
			CheckEdit->SetValue(ISUserRoleEntity::CheckExistSubSystemAccess(GroupID, SubSystem->UID));
			CheckEdit->setProperty("SubSystemUID", SubSystem->UID);
			CheckEdit->setProperty("SubSystemName", SubSystem->LocalName);
			CheckEdit->SetToolTip(LANG("AccessRights.ClickedToGiveAccessFromSubSystem"));
			connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupForm::SubSystemClicked);
			FormLayout->addRow(SubSystem->LocalName + ':', CheckEdit);
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

	QMap<QString, ISUuid> Map;
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		if (!MetaTable->IsSystem) //Если таблица является системной - пропускать
		{
			Map.insert(MetaTable->LocalListName, MetaTable->UID);
		}
	}

	for (const auto &MapItem : Map.toStdMap())
	{
		ISToolBarAccessTable *ToolBarAccessRights = new ISToolBarAccessTable(GroupID, MapItem.second, MapItem.first, ScrollArea);
		connect(ToolBarAccessRights, &ISToolBarAccessTable::actionTriggered, this, &ISUserGroupForm::TableClicked);
		FormLayout->addRow(MapItem.first + ':', ToolBarAccessRights);
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupForm::CreateSpecial()
{
	QFormLayout *FormLayout = new QFormLayout();
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(FormLayout);
	TabWidget->addTab(ScrollArea, LANG("AccessRights.SpecialRoles"));

	ISQuery qSelectSpecialGroup(QS_GROUP_ACCESS_SPECIAL_GROUP);
	if (qSelectSpecialGroup.Execute())
	{
		while (qSelectSpecialGroup.Next())
		{
			QLabel *LabelSpecialGroup = new QLabel(qSelectSpecialGroup.ReadColumn("gast_name").toString() + ':', ScrollArea);
			LabelSpecialGroup->setSizePolicy(QSizePolicy::Maximum, LabelSpecialGroup->sizePolicy().verticalPolicy());
			LabelSpecialGroup->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
			
			QHBoxLayout *Layout = new QHBoxLayout();
			Layout->addWidget(LabelSpecialGroup);
			Layout->addWidget(ISControls::CreateHorizontalLine(ScrollArea));
			FormLayout->addRow(Layout);

			ISQuery qSelectSpecial(QS_GROUP_ACCESS_SPECIAL_CHECK);
			qSelectSpecial.BindValue(":ParentUID", qSelectSpecialGroup.ReadColumn("gast_uid"));
			if (qSelectSpecial.Execute())
			{
				while (qSelectSpecial.Next())
				{
					ISUuid SpecialAccessUID = qSelectSpecial.ReadColumn("gast_uid");
					QString Name = qSelectSpecial.ReadColumn("gast_name").toString();
					QString Hint = qSelectSpecial.ReadColumn("gast_hint").toString();

					ISCheckEdit *CheckEdit = new ISCheckEdit(ScrollArea);
					CheckEdit->CreateHint(Hint);
					CheckEdit->setProperty("SpecialAccessUID", SpecialAccessUID);
					CheckEdit->setProperty("SpecialAccessName", Name);
					CheckEdit->SetValue(ISUserRoleEntity::CheckExistSpecialAccess(GroupID, SpecialAccessUID));
					connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupForm::SpecialClicked);
					FormLayout->addRow("  " + Name + ':', CheckEdit);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupForm::SubSystemClicked(const QVariant &value)
{
	ISGui::SetWaitGlobalCursor(true);
	ISUuid SubSystemUID = sender()->property("SubSystemUID"); //Идентификатор подсистемы
	QString SubSystemName = sender()->property("SubSystemName").toString();

	if (value.toBool()) //Если право было включено
	{
		ISUserRoleEntity::InsertSubSystemAccess(GroupID, SubSystemUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_ACCESS_TO_SUBSYSTEM, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, SubSystemName);
	}
	else
	{
		ISUserRoleEntity::DeleteSubSystemAccess(GroupID, SubSystemUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_DEL_ACCESS_TO_SUBSYSTEM, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, SubSystemName);
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupForm::TableClicked(QAction *Action)
{
	ISGui::SetWaitGlobalCursor(true);
	ISUuid TableUID = Action->property("TableUID"); //Идентификатор таблицы
	ISUuid AccessUID = Action->property("AccessUID"); //Идентификатор права
	QString TableName = Action->property("TableName").toString(); //Локальное наименование таблицы

	if (Action->isChecked()) //Если право было включено
	{
		ISUserRoleEntity::InsertTableAccess(GroupID, TableUID, AccessUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, TableName + " (" + Action->toolTip() + ')');
	}
	else
	{
		ISUserRoleEntity::DeleteTableAccess(GroupID, TableUID, AccessUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, TableName + " (" + Action->toolTip() + ')');
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupForm::SpecialClicked(const QVariant &value)
{
	ISGui::SetWaitGlobalCursor(true);
	ISUuid SpecialAccessUID = sender()->property("SpecialAccessUID");
	QString SpecialAccessName = sender()->property("SpecialAccessName").toString();

	if (value.toBool())
	{
		ISUserRoleEntity::InsertSpecialAccess(GroupID, SpecialAccessUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, SpecialAccessName);
	}
	else
	{
		ISUserRoleEntity::DeleteSpecialAccess(GroupID, SpecialAccessUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, SpecialAccessName);
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
