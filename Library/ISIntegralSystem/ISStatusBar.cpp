#include "StdAfx.h"
#include "ISStatusBar.h"
#include "ISLocalization.h"
#include "ISCore.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISConfig.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISSettings.h"
#include "ISConstants.h"
#include "ISMessageBox.h"
#include "ISServiceButton.h"
#include "ISSettingsDatabase.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISStatusBar::ISStatusBar(QWidget *parent) : QStatusBar(parent)
{
	if (SETTING_BOOL(CONST_UID_SETTING_STATUS_BAR_SHOWDATETIME))
	{
		CreateDateTime();
	}

	CreateConnectData();

	if (SETTING_BOOL(CONST_UID_SETTING_VIEW_SHOWWEATHERWIDGET))
	{
		ButtonWeather = new ISButtonWeather(this);
		addWidget(ButtonWeather);
	}

	if (SETTING_BOOL(CONST_UID_SETTING_VIEW_SHOWCURRENCYWIDGET))
	{
		ButtonCurrency = new ISButtonCurrency(this);
		addWidget(ButtonCurrency);
	}

	ISServiceButton *ButtonCall = new ISServiceButton(this);
	ButtonCall->setToolTip(LOCALIZATION("MakeCall"));
	ButtonCall->setIcon(BUFFER_ICONS("CallPhone"));
	connect(ButtonCall, &ISServiceButton::clicked, this, &ISStatusBar::MakeCall);
	addPermanentWidget(ButtonCall);

	ISServiceButton *ButtonTray = new ISServiceButton(this);
	ButtonTray->setToolTip(LOCALIZATION("ClickedFromCurtailApplicationTray"));
	ButtonTray->setIcon(BUFFER_ICONS("CurtailTray"));
	connect(ButtonTray, &ISServiceButton::clicked, this, &ISStatusBar::TrayClicked);
	addPermanentWidget(ButtonTray);
}
//-----------------------------------------------------------------------------
ISStatusBar::~ISStatusBar()
{

}
//-----------------------------------------------------------------------------
void ISStatusBar::CreateDateTime()
{
	ButtonDateTime = new ISPushButton(this);
	ButtonDateTime->setText(GetCurrentDateTime());
	ButtonDateTime->setToolTip(LOCALIZATION("ClickFromGoOwerToCalendar"));
	ButtonDateTime->setCursor(CURSOR_POINTING_HAND);
	ButtonDateTime->setFont(Font);
	ButtonDateTime->setFlat(true);
	connect(ButtonDateTime, &ISPushButton::clicked, this, &ISStatusBar::DateTimeClicked);
	addWidget(ButtonDateTime);

	QTimer *Timer = new QTimer(this);
	Timer->setInterval(60000);
	connect(Timer, &QTimer::timeout, this, &ISStatusBar::TimerTick);
	Timer->start();
}
//-----------------------------------------------------------------------------
void ISStatusBar::CreateConnectData()
{
	ISServiceButton *ButtonConnection = new ISServiceButton(this);
	ButtonConnection->setToolTip(LOCALIZATION("ConnectionInfoDatabase"));
	ButtonConnection->setIcon(BUFFER_ICONS("DatabaseConnection"));
	ButtonConnection->setFlat(true);
	addWidget(ButtonConnection);
	connect(ButtonConnection, &ISPushButton::clicked, [=]
	{
		QString String;
		String += LOCALIZATION("Server") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER) + "\n";
		String += LOCALIZATION("Port") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT) + "\n";
		String += LOCALIZATION("Database") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);

		ISMessageBox::ShowInformation(this, String);
	});
}
//-----------------------------------------------------------------------------
void ISStatusBar::TimerTick()
{
	ButtonDateTime->setText(GetCurrentDateTime());
}
//-----------------------------------------------------------------------------
QString ISStatusBar::GetCurrentDateTime() const
{
	return QString("%1 %2").arg(ISSystem::GetCurrentDayOfWeekName()).arg(QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_V1));
}
//-----------------------------------------------------------------------------
