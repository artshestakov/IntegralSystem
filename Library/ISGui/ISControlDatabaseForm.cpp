#include "ISControlDatabaseForm.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISScrollArea.h"
#include "ISGui.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
static QString QS_CATEGORY = PREPARE_QUERY("SELECT DISTINCT category "
										   "FROM pg_settings "
										   "ORDER BY category");
//-----------------------------------------------------------------------------
static QString QS_SETTINGS = PREPARE_QUERY("SELECT name, setting "
										   "FROM pg_settings "
										   "WHERE category = :Category "
										   "ORDER BY name");
//-----------------------------------------------------------------------------
static QString QS_TABLES = PREPARE_QUERY("SELECT tablename "
										 "FROM pg_tables "
										 "WHERE schemaname = current_schema() "
										 "ORDER BY tablename");
//-----------------------------------------------------------------------------
static QString QS_NAME_VALUE = PREPARE_QUERY("SELECT lcnm_localname "
											 "FROM _localnames "
											 "WHERE lcnm_creationuseroid = currentuseroid() "
											 "AND lcnm_tablename = :TableName "
											 "AND lcnm_fieldname = :FieldName");
//-----------------------------------------------------------------------------
static QString QS_NAME = PREPARE_QUERY("SELECT COUNT(*) "
									   "FROM _localnames "
									   "WHERE lcnm_creationuseroid = currentuseroid() "
									   "AND lcnm_tablename = :TableName "
									   "AND lcnm_fieldname = :FieldName");
//-----------------------------------------------------------------------------
static QString QI_NAME = PREPARE_QUERY("INSERT INTO _localnames (lcnm_tablename, lcnm_fieldname, lcnm_localname) "
									   "VALUES (:TableName, :FieldName, :LocalName)");
//-----------------------------------------------------------------------------
static QString QU_NAME = PREPARE_QUERY("UPDATE _localnames SET "
									   "lcnm_localname = :LocalName "
									   "WHERE lcnm_creationuseroid = currentuseroid() "
									   "AND lcnm_tablename = :TableName "
									   "AND lcnm_fieldname = :FieldName");
//-----------------------------------------------------------------------------
ISControlDatabaseForm::ISControlDatabaseForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	ISPushButton *ButtonSettings = new ISPushButton(BUFFER_ICONS("DatabaseSettings"), LANG("SettingsDatabase"), this);
	connect(ButtonSettings, &ISPushButton::clicked, &ISGui::ShowDatabaseSettings);
	GetMainLayout()->addWidget(ButtonSettings, 0, Qt::AlignRight);

	TabWidget = new QTabWidget(this);
	GetMainLayout()->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
ISControlDatabaseForm::~ISControlDatabaseForm()
{
	//Сохраняем локальные имена полей
	for (ISLineEdit *LineEdit : Edits)
	{
		if (!LineEdit->GetModificationFlag())
		{
			continue;
		}

		QString TableName = LineEdit->property("TableName").toString();
		QString FieldName = LineEdit->property("FieldName").toString();
		QVariant Value = LineEdit->GetValue();

		ISQuery qSelect(QS_NAME);
		qSelect.BindValue(":TableName", TableName);
		qSelect.BindValue(":FieldName", FieldName);
		if (qSelect.ExecuteFirst())
		{
			if (qSelect.ReadColumn("count").toBool())
			{
				ISQuery qUpdate(QU_NAME);
				qUpdate.BindValue(":LocalName", Value);
				qUpdate.BindValue(":TableName", TableName);
				qUpdate.BindValue(":FieldName", FieldName);
				qUpdate.Execute();
			}
			else
			{
				if (!Value.toString().isEmpty())
				{
					ISQuery qInsert(QI_NAME);
					qInsert.BindValue(":TableName", TableName);
					qInsert.BindValue(":FieldName", FieldName);
					qInsert.BindValue(":LocalName", Value);
					qInsert.Execute();
				}
			}
			PMetaField *MetaField = ISMetaData::Instance().GetMetaTable(TableName)->GetField(FieldName);
			MetaField->LabelName = Value.toString();
			MetaField->LocalListName = MetaField->LabelName;
		}
	}
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::LoadData()
{
	ISGui::SetWaitGlobalCursor(true);
	CreateGeneralTab();
	CreatePGSettings();
	CreateStatisticTablesForm();
	CreateSettingLocalNameFields();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreateGeneralTab()
{
	QFormLayout *FormLayout = new QFormLayout();

	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(FormLayout);
	TabWidget->addTab(ScrollArea, LANG("GeneralInformation"));

	FormLayout->addRow(LANG("DatabaseName") + ':', new QLabel(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE), ScrollArea));
	FormLayout->addRow(LANG("DatabaseSize") + ':', new QLabel(ISDatabase::Instance().GetCurrentDatabaseSize(), ScrollArea));
	FormLayout->addRow(LANG("VersionPostgreSQL") + ':', new QLabel(ISDatabase::Instance().GetVersionPostgres(), ScrollArea));
	FormLayout->addRow(LANG("ServerStartTime") + ':', new QLabel(ISDatabase::Instance().GetStartTimeServer(), ScrollArea));
	FormLayout->addRow(LANG("ServerLoadConfigrationTime") + ':', new QLabel(ISDatabase::Instance().GetLoadConfigurationTime(), ScrollArea));
	FormLayout->addRow(LANG("InetClientAddress") + ':', new QLabel(ISDatabase::Instance().GetInetClientAddress(), ScrollArea));
	FormLayout->addRow(LANG("InetServerAddress") + ':', new QLabel(ISDatabase::Instance().GetInetServerAddress(), ScrollArea));
	FormLayout->addRow(LANG("CodeServerProcess") + ':', new QLabel(QString::number(ISDatabase::Instance().GetServerProcessID()), ScrollArea));
	FormLayout->addRow("LC_COLLATE:", new QLabel(ISDatabase::Instance().GetDatabaseCollate(), ScrollArea));
	FormLayout->addRow("CType:", new QLabel(ISDatabase::Instance().GetDatabaseCType(), ScrollArea));
	FormLayout->addRow(LANG("ClusterDirectory") + ':', new QLabel(ISDatabase::Instance().GetDatabaseDataDirectory()));
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreatePGSettings()
{
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	TabWidget->addTab(ScrollArea, LANG("PG_Settings"));

	ISQuery qSelectCategory(QS_CATEGORY);
	if (qSelectCategory.Execute())
	{
		while (qSelectCategory.Next())
		{
			QString Category = qSelectCategory.ReadColumn("category").toString();

			QFormLayout *FormLayout = new QFormLayout();
			QGroupBox *GroupBox = new QGroupBox(ScrollArea);
			GroupBox->setTitle(Category);
			GroupBox->setLayout(FormLayout);
			ScrollArea->widget()->layout()->addWidget(GroupBox);

			ISQuery qSelectSettings(QS_SETTINGS);
			qSelectSettings.BindValue(":Category", Category);
			if (qSelectSettings.Execute())
			{
				while (qSelectSettings.Next())
				{
					QString SettingName = qSelectSettings.ReadColumn("name").toString();
					QString SettingValue = qSelectSettings.ReadColumn("setting").toString();

					QLabel *LabelValue = new QLabel(GroupBox);
					LabelValue->setText(SettingValue);
					ISGui::SetFontWidgetBold(LabelValue, true);
					FormLayout->addRow(SettingName + ':', LabelValue);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreateStatisticTablesForm()
{
	QFormLayout *FormLayout = new QFormLayout();
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(FormLayout);
	TabWidget->addTab(ScrollArea, LANG("StatisticTables"));

	ISQuery qSelect(QS_TABLES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString TableName = qSelect.ReadColumn("tablename").toString();

			ISPushButton *ButtonTable = new ISPushButton(ScrollArea);
			ButtonTable->setText(LANG("Details"));
			ButtonTable->setProperty("TableName", TableName);
			ButtonTable->setSizePolicy(QSizePolicy::Maximum, ButtonTable->sizePolicy().verticalPolicy());
			connect(ButtonTable, &ISPushButton::clicked, this, &ISControlDatabaseForm::DetailsClicked);
			FormLayout->addRow(TableName + ':', ButtonTable);
		}
	}
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreateSettingLocalNameFields()
{
	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	TabWidget->addTab(ScrollArea, LANG("SettingNameFields"));

	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		if (MetaTable->IsSystem)
		{
			continue;
		}

		QFormLayout *FormLayout = new QFormLayout();
		QGroupBox *GroupBox = new QGroupBox(MetaTable->LocalListName, ScrollArea);
		GroupBox->setLayout(FormLayout);
		ScrollArea->widget()->layout()->addWidget(GroupBox);

		for (PMetaField *MetaField : MetaTable->Fields)
		{
			ISLineEdit *LineEdit = new ISLineEdit(GroupBox);
			LineEdit->setSizePolicy(QSizePolicy::Maximum, LineEdit->sizePolicy().verticalPolicy());
			LineEdit->setProperty("TableName", MetaTable->Name);
			LineEdit->setProperty("FieldName", MetaField->Name.toLower());
			FormLayout->addRow(MetaField->LabelName + ":", LineEdit);
			Edits.emplace_back(LineEdit);

			ISQuery qSelect(QS_NAME_VALUE);
			qSelect.BindValue(":TableName", MetaTable->Name);
			qSelect.BindValue(":FieldName", MetaField->Name.toLower());
			if (qSelect.ExecuteFirst())
			{
				if (qSelect.GetCountResultRows() > 0)
				{
					LineEdit->SetValue(qSelect.ReadColumn("lcnm_localname"));
					LineEdit->SetModificationFlag(false);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::DetailsClicked()
{
	QString TableName = sender()->property("TableName").toString();
	QString Content = LANG("TableName") + ": " + TableName + "\n" +
		LANG("Size") + ": " + ISDatabase::Instance().GetTableSize(TableName) + "\n" +
		LANG("SizeWithIndices") + ": " + ISDatabase::Instance().GetTableSizeWithIndices(TableName) + "\n" +
		LANG("RowCount") + ": " + QString::number(ISDatabase::Instance().GetTableRowCount(TableName));
	ISMessageBox::ShowInformation(this, Content);
}
//-----------------------------------------------------------------------------
