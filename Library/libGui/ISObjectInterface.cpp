#include "StdAfx.h"
#include "ISObjectInterface.h"
#include "ISDefines.h"
#include "ISLicense.h"
//-----------------------------------------------------------------------------
ISObjectInterface::ISObjectInterface(QObject *parent)
	: QObject(parent),
	LicenseName(ISLicense::GetInstance().GetName())
{

}
//-----------------------------------------------------------------------------
ISObjectInterface::~ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
QIcon ISObjectInterface::GetIcon(const QString &IconName) const
{
	return QIcon(":_" + LicenseName + "/" + IconName + ".png");
}
//-----------------------------------------------------------------------------
