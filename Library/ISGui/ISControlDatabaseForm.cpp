#include "ISControlDatabaseForm.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISScrollArea.h"
#include "ISGui.h"
#include "ISQuery.h"
#include "ISPGSettingsForm.h"
#include "ISStatisticTablesForm.h"
#include "ISSettingTableNameFieldsForm.h"
//-----------------------------------------------------------------------------
ISControlDatabaseForm::ISControlDatabaseForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	ISPushButton *ButtonSettings = new ISPushButton(this);
	ButtonSettings->setText(LANG("SettingsDatabase"));
	ButtonSettings->setIcon(BUFFER_ICONS("DatabaseSettings"));
	connect(ButtonSettings, &ISPushButton::clicked, &ISGui::ShowDatabaseSettings);
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

	//Наименование базы
	FormLayout->addRow(LANG("DatabaseName") + ':', new QLabel(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE), ScrollArea));

	//Размер базы данных
	FormLayout->addRow(LANG("DatabaseSize") + ':', new QLabel(ISDatabase::Instance().GetCurrentDatabaseSize(), ScrollArea));

	//Версия PostgreSQL
	FormLayout->addRow(LANG("VersionPostgreSQL") + ':', new QLabel(ISDatabase::Instance().GetVersionPostgres(), ScrollArea));

	//Время запуска сервера
	FormLayout->addRow(LANG("ServerStartTime") + ':', new QLabel(ISDatabase::Instance().GetStartTimeServer(), ScrollArea));

	//Время загрузки конфигурации
	FormLayout->addRow(LANG("ServerLoadConfigrationTime") + ':', new QLabel(ISDatabase::Instance().GetLoadConfigurationTime(), ScrollArea));

	//Адресс локального соединения
	FormLayout->addRow(LANG("InetClientAddress") + ':', new QLabel(ISDatabase::Instance().GetInetClientAddress(), ScrollArea));

	//Адрес удаленного соединения
	FormLayout->addRow(LANG("InetServerAddress") + ':', new QLabel(ISDatabase::Instance().GetInetServerAddress(), ScrollArea));

	//Код серверного процесса обслуживающего текущий сеанс
	FormLayout->addRow(LANG("CodeServerProcess") + ':', new QLabel(QString::number(ISDatabase::Instance().GetServerProcessID()), ScrollArea));

	//LCCollate базы
	FormLayout->addRow("LC_COLLATE:", new QLabel(ISDatabase::Instance().GetDatabaseCollate(), ScrollArea));

	//CType базы
	FormLayout->addRow("CType:", new QLabel(ISDatabase::Instance().GetDatabaseCType(), ScrollArea));

	//Путь к кластеру на сервере СУБД
	FormLayout->addRow(LANG("ClusterDirectory") + ':', new QLabel(ISDatabase::Instance().GetDatabaseDataDirectory()));
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreatePGSettings()
{
	TabWidget->addTab(new ISPGSettingsForm(TabWidget), LANG("PG_Settings"));
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreateStatisticTablesForm()
{
	TabWidget->addTab(new ISStatisticTablesForm(TabWidget), LANG("StatisticTables"));
}
//-----------------------------------------------------------------------------
void ISControlDatabaseForm::CreateSettingLocalNameFields()
{
	TabWidget->addTab(new ISSettingTableNameFieldsForm(TabWidget), LANG("SettingNameFields"));
}
//-----------------------------------------------------------------------------
