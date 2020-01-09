#include "StdAfx.h"
#include "CGConfiguratorBase.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
CGConfiguratorBase::CGConfiguratorBase() : QObject()
{
	
}
//-----------------------------------------------------------------------------
CGConfiguratorBase::~CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorBase::Progress(const QString &ProgressName, int Step, int Steps)
{
	emit ProgressMessage(QString("Progress \"%1\": %2 of %3").arg(ProgressName).arg(Step + 1).arg(Steps));
}
//-----------------------------------------------------------------------------
