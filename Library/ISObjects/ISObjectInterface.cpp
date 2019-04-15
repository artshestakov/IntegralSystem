#include "StdAfx.h"
#include "ISObjectInterface.h"
#include "EXDefines.h"
#include "ISLicense.h"
//-----------------------------------------------------------------------------
ISObjectInterface::ISObjectInterface(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------
ISObjectInterface::~ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
QIcon ISObjectInterface::GetIcon(const QString &IconName) const
{
	return QIcon(":_" + ISLicense::GetInstance().GetName() + "/" + IconName + ".png");
}
//-----------------------------------------------------------------------------
