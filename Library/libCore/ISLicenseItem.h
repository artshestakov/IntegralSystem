#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISLicenseItem
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
