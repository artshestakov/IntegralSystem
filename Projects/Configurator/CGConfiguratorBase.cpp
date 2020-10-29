#include "CGConfiguratorBase.h"
#include "ISDebug.h"
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
void CGConfiguratorBase::Progress(const QString &ObjectTypeName, size_t Step, size_t TotalStep, const QString &Description)
{
	ISDEBUG_L(QString("Progress \"%1\": %2 of %3%4").arg(ObjectTypeName).arg(Step + 1).arg(TotalStep).arg(!Description.isEmpty() ? ". " + Description : QString()));
}
//-----------------------------------------------------------------------------
