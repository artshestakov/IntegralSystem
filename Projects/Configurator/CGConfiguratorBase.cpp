#include "CGConfiguratorBase.h"
#include "ISLogger.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
CGConfiguratorBase::CGConfiguratorBase()
	: QObject(),
	ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
CGConfiguratorBase::~CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
QString CGConfiguratorBase::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void CGConfiguratorBase::Progress(const QString &ObjectTypeName, int Step, int Steps)
{
	ISLOGGER_UNKNOWN(QString("Progress \"%1\": %2 of %3").arg(ObjectTypeName).arg(Step + 1).arg(Steps));
}
//-----------------------------------------------------------------------------
