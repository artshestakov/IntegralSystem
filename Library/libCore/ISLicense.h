#pragma once
//-----------------------------------------------------------------------------
#include <QString>
//-----------------------------------------------------------------------------
class ISLicense
{
public:
	ISLicense();
	virtual ~ISLicense();

	static QByteArray GetKey(const QString &ConfUID, const QString &SerialUID);
};
//-----------------------------------------------------------------------------
