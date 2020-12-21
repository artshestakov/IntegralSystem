#include "ISUserGroupForm.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISScrollArea.h"
#include "ISMetaSystemsEntity.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISUserRoleEntity.h"
#include "ISControls.h"
#include "ISProtocol.h"
#include "ISGui.h"
#include "ISTcpQuery.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_TABLE_TYPE = PREPARE_QUERY("SELECT gatt_id, gatt_name, gatt_icon "
														  "FROM _groupaccesstabletype "
														  "ORDER BY gatt_order");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL_GROUP = PREPARE_QUERY("SELECT gast_uid, gast_name "
															 "FROM _groupaccessspecialtype "
															 "WHERE gast_parent IS NULL "
															 "ORDER BY gast_order");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL_CHECK = PREPARE_QUERY("SELECT gast_id, gast_name, gast_hint "
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
}
//-----------------------------------------------------------------------------
ISUserGroupForm::~ISUserGroupForm()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();

	ISTcpQuery qGetGroupRights(API_GET_GROUP_RIGHTS);
	qGetGroupRights.BindValue("GroupID", GroupID);
	if (qGetGroupRights.Execute())
	{
		QVariantMap ResultMap = qGetGroupRights.TakeAnswer();
		SubSystems = ResultMap["SubSystems"].toList();

		CreateSubSystems();
		CreateTables();
		CreateSpecial();
	}
	else
	{
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

	for (ISMetaSystem *MetaSystem : ISMetaSystemsEntity::Instance().GetSystems()) //����� ���� ������
	{
		QGroupBox *GroupBox = new QGroupBox(MetaSystem->LocalName, ScrollArea);
		GroupBox->setLayout(new QVBoxLayout());
		GroupBox->setStyleSheet(BUFFER_STYLE_SHEET("QGroupBoxAccessSubSystem"));
		ScrollArea->widget()->layout()->addWidget(GroupBox);

		for (ISMetaSubSystem *SubSystem : MetaSystem->SubSystems) //����� ���� ��������� ������� �������
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

	//����������� ���� ���� �� �������
	ISVectorMap AccessTypeVector;
	ISQuery qSelectAccess(QS_GROUP_ACCESS_TABLE_TYPE);
	if (qSelectAccess.Execute())
	{
		while (qSelectAccess.Next())
		{
			AccessTypeVector.emplace_back(QVariantMap
			{
				{ "AccessID", qSelectAccess.ReadColumn("gatt_id") },
				{ "Name", qSelectAccess.ReadColumn("gatt_name") },
				{ "Icon", qSelectAccess.ReadColumn("gatt_icon") }
			});
		}
	}

	//����������� �� ����-������ ��� �� ��������� �������
	ISStringMap Map;
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		if (!MetaTable->IsSystem) //���� ������� �������� ��������� - ����������
		{
			Map[MetaTable->LocalListName] = MetaTable->Name;
		}
	}

	for (const auto &MapItem : Map) //������� �������
	{
		QToolBar *ToolBar = new QToolBar(ScrollArea);
		ToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		ToolBar->setIconSize(ISDefines::Gui::SIZE_20_20);
		connect(ToolBar, &QToolBar::actionTriggered, this, &ISUserGroupForm::TableClicked);
		FormLayout->addRow(MapItem.first + ':', ToolBar);

		for (const QVariantMap &VariantMap : AccessTypeVector) //������� ���� ����
		{
			QAction *Action = ToolBar->addAction(BUFFER_ICONS(VariantMap["Icon"].toString()), VariantMap["Name"].toString());
			Action->setCheckable(true);
			Action->setProperty("TableName", MapItem.second);
			Action->setProperty("LocalName", MapItem.first);
			Action->setProperty("AccessID", VariantMap["AccessID"]);
			Action->setChecked(ISUserRoleEntity::CheckExistTableAccess(GroupID, MapItem.second, VariantMap["AccessID"].toInt()));
			ToolBar->widgetForAction(Action)->setCursor(CURSOR_POINTING_HAND);
		}
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
					int SpecialAccessID = qSelectSpecial.ReadColumn("gast_id").toInt();
					QString Name = qSelectSpecial.ReadColumn("gast_name").toString();
					QString Hint = qSelectSpecial.ReadColumn("gast_hint").toString();

					ISCheckEdit *CheckEdit = new ISCheckEdit(ScrollArea);
					CheckEdit->CreateHint(Hint);
					CheckEdit->setProperty("SpecialAccessID", SpecialAccessID);
					CheckEdit->setProperty("SpecialAccessName", Name);
					CheckEdit->SetValue(ISUserRoleEntity::CheckExistSpecialAccess(GroupID, SpecialAccessID));
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
	QString TableName = Action->property("TableName").toString(); //��� �������
	QString LocalName = Action->property("LocalName").toString(); //��������� ��� �������
	int AccessID = Action->property("AccessID").toInt(); //������������� �����

	if (Action->isChecked()) //���� ����� ���� ��������
	{
		ISUserRoleEntity::InsertTableAccess(GroupID, TableName, AccessID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, LocalName);
	}
	else
	{
		ISUserRoleEntity::DeleteTableAccess(GroupID, TableName, AccessID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, LocalName);
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupForm::SpecialClicked(const QVariant &value)
{
	ISGui::SetWaitGlobalCursor(true);
	int SpecialAccessID = sender()->property("SpecialAccessID").toInt();
	QString SpecialAccessName = sender()->property("SpecialAccessName").toString();

	if (value.toBool())
	{
		ISUserRoleEntity::InsertSpecialAccess(GroupID, SpecialAccessID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, SpecialAccessName);
	}
	else
	{
		ISUserRoleEntity::DeleteSpecialAccess(GroupID, SpecialAccessID);
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL, "_UserGroup", ISMetaData::Instance().GetMetaTable("_UserGroup")->LocalListName, GroupID, SpecialAccessName);
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
