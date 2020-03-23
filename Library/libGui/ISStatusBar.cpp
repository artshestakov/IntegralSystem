#include "ISStatusBar.h"
#include "ISLocalization.h"
#include "ISCore.h"
#include "ISBuffer.h"
#include "ISConfig.h"
#include "ISConstants.h"
#include "ISMessageBox.h"
#include "ISButtons.h"
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
}
//-----------------------------------------------------------------------------
ISStatusBar::~ISStatusBar()
{

}
//-----------------------------------------------------------------------------
