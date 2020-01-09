#pragma once
//-----------------------------------------------------------------------------
#include <QString>
//-----------------------------------------------------------------------------
class EXLicense
{
public:
	EXLicense();
	virtual ~EXLicense();

	static QByteArray GetKey(const QString &ConfUID, const QString &SerialUID);
};
//-----------------------------------------------------------------------------
