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
	ISServiceButton *ButtonConnection = new ISServiceButton(this);
	ButtonConnection->setToolTip(LOCALIZATION("ConnectionInfoDatabase"));
	ButtonConnection->setIcon(BUFFER_ICONS("DatabaseConnection"));
	ButtonConnection->setFlat(true);
	addWidget(ButtonConnection);
	connect(ButtonConnection, &ISPushButton::clicked, [=]
	{
		ISMessageBox::ShowInformation(this, LOCALIZATION("Server") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER) + "\n" +
											LOCALIZATION("Port") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT) + "\n" +
											LOCALIZATION("Database") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE));
	});

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
