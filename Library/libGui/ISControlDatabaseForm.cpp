#include "ISControlDatabaseForm.h"
#include "EXDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISScrollArea.h"
#include "ISGui.h"
#include "ISQuery.h"
#include "ISPGSettingsForm.h"
#include "ISStatisticTablesForm.h"
#include "ISDistFileListForm.h"
//-----------------------------------------------------------------------------
static QString QS_SETTING_DATABASE_ID = PREPARE_QUERY("SELECT sgdb_id FROM _settingsdatabase WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
ISControlDatabaseForm::ISControlDatabaseForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	ISPushButton *ButtonSettings = new ISPushButton(this);
	ButtonSettings->setText(LANG("SettingsDatabase"));
	ButtonSettings->setIcon(BUFFER_ICONS("DatabaseSettings"));
	connect(ButtonSettings, &ISPushButton::clicked, this, &ISControlDatabaseForm::ShowDatabaseSettings);
	GetMainLayout()->addWidget(ButtonSettings, 0, Qt::AlignRight);

	TabWidget = new QTabWidget(this);
	GetMainLayout()->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
ISControlDatabaseForm::~ISControlDatabaseForm()
{

}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::LoadData()
{
	ISGui::SetWaitGlobalCursor(true);
	CreateGeneralTab();
	CreatePGSettings();
	CreateStatisticTablesForm();
	CreateDistFilesForm();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreateGeneralTab()
{
	QFormLayout *FormLayout = new QFormLayout();

	ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
	ScrollArea->widget()->setLayout(FormLayout);
	TabWidget->addTab(ScrollArea, LANG("GeneralInformation"));

	//Наименование базы
	FormLayout->addRow(LANG("DatabaseName") + ':', new QLabel(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE), ScrollArea));

	//Размер базы данных
	FormLayout->addRow(LANG("DatabaseSize") + ':', new QLabel(ISDatabase::GetInstance().GetCurrentDatabaseSize(), ScrollArea));

	//Версия PostgreSQL
	FormLayout->addRow(LANG("VersionPostgreSQL") + ':', new QLabel(ISDatabase::GetInstance().GetVersionPostgres(), ScrollArea));

	//Время запуска сервера
	FormLayout->addRow(LANG("ServerStartTime") + ':', new QLabel(ISDatabase::GetInstance().GetStartTimeServer(), ScrollArea));

	//Время загрузки конфигурации
	FormLayout->addRow(LANG("ServerLoadConfigrationTime") + ':', new QLabel(ISDatabase::GetInstance().GetLoadConfigurationTime(), ScrollArea));

	//Адресс локального соединения
	FormLayout->addRow(LANG("InetClientAddress") + ':', new QLabel(ISDatabase::GetInstance().GetInetClientAddress(), ScrollArea));

	//Адрес удаленного соединения
	FormLayout->addRow(LANG("InetServerAddress") + ':', new QLabel(ISDatabase::GetInstance().GetInetServerAddress(), ScrollArea));

	//Код серверного процесса обслуживающего текущий сеанс
	FormLayout->addRow(LANG("CodeServerProcess") + ':', new QLabel(QString::number(ISDatabase::GetInstance().GetServerProcessID()), ScrollArea));

	//LCCollate базы
	FormLayout->addRow("LC_COLLATE:", new QLabel(ISDatabase::GetInstance().GetDatabaseCollate(), ScrollArea));

	//CType базы
	FormLayout->addRow("CType:", new QLabel(ISDatabase::GetInstance().GetDatabaseCType(), ScrollArea));

	//Путь к кластеру на сервере СУБД
	FormLayout->addRow(LANG("ClusterDirectory") + ':', new QLabel(ISDatabase::GetInstance().GetDatabaseDataDirectory()));
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreatePGSettings()
{
	ISPGSettingsForm *PGSettingsForm = new ISPGSettingsForm(TabWidget);
	TabWidget->addTab(PGSettingsForm, LANG("PG_Settings"));
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreateStatisticTablesForm()
{
	ISStatisticTablesForm *StatisticTablesForm = new ISStatisticTablesForm(TabWidget);
	TabWidget->addTab(StatisticTablesForm, BUFFER_ICONS("TablesStatistics"), LANG("StatisticTables"));
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreateDistFilesForm()
{
	ISDistFileListForm *DistFileListForm = new ISDistFileListForm(TabWidget);
	DistFileListForm->LoadData();
	TabWidget->addTab(DistFileListForm, BUFFER_ICONS("Updates"), LANG("Updates"));
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::ShowDatabaseSettings()
{
	ISGui::SetWaitGlobalCursor(true);
	ISQuery qSelectID(QS_SETTING_DATABASE_ID);
	qSelectID.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	if (qSelectID.ExecuteFirst())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_SettingsDatabase", qSelectID.ReadColumn("sgdb_id").toInt())->showMaximized();
	}
}
//-----------------------------------------------------------------------------
