#include "CGConfiguratorBase.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
CGConfiguratorBase::CGConfiguratorBase() : QObject()
{
	
}
//-----------------------------------------------------------------------------
CGConfiguratorBase::~CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorBase::Progress(const QString &ObjectTypeName, int Step, int Steps)
{
	ISLOGGER_UNKNOWN(QString("Progress \"%1\": %2 of %3").arg(ObjectTypeName).arg(Step + 1).arg(Steps));
}
//-----------------------------------------------------------------------------
