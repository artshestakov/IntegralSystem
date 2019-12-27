#include "StdAfx.h"
#include "ISLicense.h"
//-----------------------------------------------------------------------------
ISLicense::ISLicense()
{
	
}
//-----------------------------------------------------------------------------
ISLicense::~ISLicense()
{

}
//-----------------------------------------------------------------------------
QByteArray ISLicense::GetKey(const QString &ConfUID, const QString &SerialUID)
{
	return QCryptographicHash::hash(QString(ConfUID + "_" + SerialUID).toUtf8(), QCryptographicHash::Md5).toHex();
}
//-----------------------------------------------------------------------------
