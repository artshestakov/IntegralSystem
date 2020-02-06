#include "ISUserGroupWidget.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISScrollArea.h"
#include "ISMetaSystemsEntity.h"
#include "ISPushButton.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISToolBarAccessTable.h"
#include "ISMetaData.h"
#include "ISCheckEdit.h"
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
ISUserGroupWidget::ISUserGroupWidget(int group_id, const QString &group_name, QWidget *parent) : QWidget(parent)
{
	GroupID = group_id;

	QVBoxLayout *Layout = new QVBoxLayout();
	setLayout(Layout);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	Layout->addLayout(LayoutTitle);

	QLabel *LabelInfo = new QLabel(this);
	LabelInfo->setText(LANG("AccessRights.SettingAcceessFromGroup") + ':');
	LayoutTitle->addWidget(LabelInfo);

	QLabel *LabelGroup = new QLabel(this);
	LabelGroup->setText(group_name);
	ISGui::SetFontWidgetBold(LabelGroup, true);
	LayoutTitle->addWidget(LabelGroup);

	LayoutTitle->addStretch();

	TabWidget = new QTabWidget(this);
	Layout->addWidget(TabWidget);

	CreateSubSystems();
	CreateTables();
	CreateSpecial();
}
//-----------------------------------------------------------------------------
ISUserGroupWidget::~ISUserGroupWidget()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupWidget::CreateSubSystems()
{
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	TabWidget->addTab(ScrollArea, LANG("AccessRights.SubSystems"));

	QVector<ISMetaSystem*> Systems = ISMetaSystemsEntity::GetInstance().GetSystems();
	for (ISMetaSystem *MetaSystem : Systems) //Обход всех систем
	{
		QFormLayout *FormLayout = new QFormLayout();

		QGroupBox *GroupBox = new QGroupBox(ScrollArea);
		GroupBox->setTitle(MetaSystem->GetLocalName());
		GroupBox->setLayout(FormLayout);
		GroupBox->setStyleSheet(STYLE_SHEET("QGroupBoxAccessSubSystem"));
		ScrollArea->widget()->layout()->addWidget(GroupBox);

		QVector<ISMetaSubSystem*> SubSystems = MetaSystem->GetSubSystems();
		for (ISMetaSubSystem *SubSystem : SubSystems) //Обход всех подсистем текущей системы
		{
			ISCheckEdit *CheckEdit = new ISCheckEdit(GroupBox);
			CheckEdit->SetValue(ISUserRoleEntity::CheckExistSubSystemAccess(GroupID, SubSystem->GetUID()));
			CheckEdit->setProperty("SubSystemUID", SubSystem->GetUID());
			CheckEdit->setProperty("SubSystemName", SubSystem->GetLocalName());
			CheckEdit->SetToolTip(LANG("AccessRights.ClickedToGiveAccessFromSubSystem"));
			connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupWidget::SubSystemClicked);
			FormLayout->addRow(SubSystem->GetLocalName() + ':', CheckEdit);
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupWidget::CreateTables()
{
	QFormLayout *FormLayout = new QFormLayout();
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(FormLayout);
	TabWidget->addTab(ScrollArea, LANG("AccessRights.Tables"));

	QMap<QString, ISUuid> Map;
	QList<PMetaClassTable*> Tables = ISMetaData::GetInstanse().GetTables();
	for (PMetaClassTable *MetaTable : Tables)
	{
		if (!MetaTable->GetIsSystem()) //Если таблица является системной - пропускать
		{
			Map.insert(MetaTable->GetLocalListName(), MetaTable->GetUID());
		}
	}

	for (const auto &MapItem : Map.toStdMap())
	{
		ISToolBarAccessTable *ToolBarAccessRights = new ISToolBarAccessTable(GroupID, MapItem.second, MapItem.first, ScrollArea);
		connect(ToolBarAccessRights, &ISToolBarAccessTable::actionTriggered, this, &ISUserGroupWidget::TableClicked);
		FormLayout->addRow(MapItem.first + ':', ToolBarAccessRights);
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupWidget::CreateSpecial()
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
			ISUuid SpecialGroupUID = qSelectSpecialGroup.ReadColumn("gast_uid");
			QString SpecialGroupName = qSelectSpecialGroup.ReadColumn("gast_name").toString();

			QLabel *LabelSpecialGroup = new QLabel(ScrollArea);
			LabelSpecialGroup->setText(SpecialGroupName + ':');
			LabelSpecialGroup->setSizePolicy(QSizePolicy::Maximum, LabelSpecialGroup->sizePolicy().verticalPolicy());
			LabelSpecialGroup->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
			
			QHBoxLayout *Layout = new QHBoxLayout();
			Layout->addWidget(LabelSpecialGroup);
			Layout->addWidget(ISControls::CreateHorizontalLine(ScrollArea));
			FormLayout->addRow(Layout);

			ISQuery qSelectSpecial(QS_GROUP_ACCESS_SPECIAL_CHECK);
			qSelectSpecial.BindValue(":ParentUID", SpecialGroupUID);
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
					connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupWidget::SpecialClicked);
					FormLayout->addRow("  " + Name + ':', CheckEdit);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserGroupWidget::SubSystemClicked(const QVariant &value)
{
	ISGui::SetWaitGlobalCursor(true);

	ISCheckEdit *CheckEdit = dynamic_cast<ISCheckEdit*>(sender());
	ISUuid SubSystemUID = CheckEdit->property("SubSystemUID"); //Идентификатор подсистемы
	QString SubSystemName = CheckEdit->property("SubSystemName").toString();

	if (value.toBool()) //Если право было включено
	{
		ISUserRoleEntity::InsertSubSystemAccess(GroupID, SubSystemUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_ACCESS_TO_SUBSYSTEM, "_UserGroup", ISMetaData::GetInstanse().GetMetaTable("_UserGroup")->GetLocalListName(), GroupID, SubSystemName);
	}
	else
	{
		ISUserRoleEntity::DeleteSubSystemAccess(GroupID, SubSystemUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_DEL_ACCESS_TO_SUBSYSTEM, "_UserGroup", ISMetaData::GetInstanse().GetMetaTable("_UserGroup")->GetLocalListName(), GroupID, SubSystemName);
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupWidget::TableClicked(QAction *Action)
{
	ISGui::SetWaitGlobalCursor(true);
	ISUuid TableUID = Action->property("TableUID"); //Идентификатор таблицы
	ISUuid AccessUID = Action->property("AccessUID"); //Идентификатор права
	QString TableName = Action->property("TableName").toString(); //Локальное наименование таблицы

	if (Action->isChecked()) //Если право было включено
	{
		ISUserRoleEntity::InsertTableAccess(GroupID, TableUID, AccessUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE, "_UserGroup", ISMetaData::GetInstanse().GetMetaTable("_UserGroup")->GetLocalListName(), GroupID, TableName + " (" + Action->toolTip() + ')');
	}
	else
	{
		ISUserRoleEntity::DeleteTableAccess(GroupID, TableUID, AccessUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE, "_UserGroup", ISMetaData::GetInstanse().GetMetaTable("_UserGroup")->GetLocalListName(), GroupID, TableName + " (" + Action->toolTip() + ')');
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupWidget::SpecialClicked(const QVariant &value)
{
	ISGui::SetWaitGlobalCursor(true);

	ISCheckEdit *CheckEdit = dynamic_cast<ISCheckEdit*>(sender());
	ISUuid SpecialAccessUID = CheckEdit->property("SpecialAccessUID");
	QString SpecialAccessName = CheckEdit->property("SpecialAccessName").toString();

	if (value.toBool())
	{
		ISUserRoleEntity::InsertSpecialAccess(GroupID, SpecialAccessUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL, "_UserGroup", ISMetaData::GetInstanse().GetMetaTable("_UserGroup")->GetLocalListName(), GroupID, SpecialAccessName);
	}
	else
	{
		ISUserRoleEntity::DeleteSpecialAccess(GroupID, SpecialAccessUID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL, "_UserGroup", ISMetaData::GetInstanse().GetMetaTable("_UserGroup")->GetLocalListName(), GroupID, SpecialAccessName);
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
