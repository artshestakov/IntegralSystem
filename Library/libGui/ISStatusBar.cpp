#include "ISStatusBar.h"
#include "ISLocalization.h"
#include "ISCore.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISConfig.h"
#include "EXDefines.h"
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
	ButtonConnection->setToolTip(LANG("ConnectionInfoDatabase"));
	ButtonConnection->setIcon(BUFFER_ICONS("DatabaseConnection"));
	ButtonConnection->setFlat(true);
	addWidget(ButtonConnection);
	connect(ButtonConnection, &ISPushButton::clicked, [=]
	{
		ISMessageBox::ShowInformation(this, LANG("Server") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER) + "\n" +
											LANG("Port") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT) + "\n" +
											LANG("Database") + ": " + CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE));
	});

	ISServiceButton *ButtonCall = new ISServiceButton(this);
	ButtonCall->setToolTip(LANG("MakeCall"));
	ButtonCall->setIcon(BUFFER_ICONS("CallPhone"));
	connect(ButtonCall, &ISServiceButton::clicked, this, &ISStatusBar::MakeCall);
	addPermanentWidget(ButtonCall);

	ISServiceButton *ButtonTray = new ISServiceButton(this);
	ButtonTray->setToolTip(LANG("ClickedFromCurtailApplicationTray"));
	ButtonTray->setIcon(BUFFER_ICONS("CurtailTray"));
	connect(ButtonTray, &ISServiceButton::clicked, this, &ISStatusBar::TrayClicked);
	addPermanentWidget(ButtonTray);
}
//-----------------------------------------------------------------------------
ISStatusBar::~ISStatusBar()
{

}
//-----------------------------------------------------------------------------
