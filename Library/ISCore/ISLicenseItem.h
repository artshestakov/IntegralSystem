#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLicenseItem
{
public:
	ISLicenseItem();
	virtual ~ISLicenseItem();

	ISUuid UID;
	QString Name;
	QString LocalName;
	QString ClassName;
	QString DesktopForm;
	QString IncomingCallForm;
};
//-----------------------------------------------------------------------------
