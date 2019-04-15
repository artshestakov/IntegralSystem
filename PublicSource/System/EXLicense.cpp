#include "StdAfx.h"
#include "EXLicense.h"
//-----------------------------------------------------------------------------
EXLicense::EXLicense()
{
	
}
//-----------------------------------------------------------------------------
EXLicense::~EXLicense()
{

}
//-----------------------------------------------------------------------------
QByteArray EXLicense::GetKey(const QString &ConfUID, const QString &SerialUID)
{
	return QCryptographicHash::hash(QString(ConfUID + "_" + SerialUID).toUtf8(), QCryptographicHash::Md5).toHex();
}
//-----------------------------------------------------------------------------
