#pragma once
//-----------------------------------------------------------------------------
#include "ISUuid.h"
//-----------------------------------------------------------------------------
struct ISLicenseItem
{
	ISUuid UID;
	QString Name;
	QString LocalName;
	QString ClassName;
	QString DesktopForm;
	QString IncomingCallForm;
};
//-----------------------------------------------------------------------------
