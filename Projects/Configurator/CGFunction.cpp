#include "StdAfx.h"
#include "CGFunction.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
CGFunction::CGFunction() : QObject()
{

}
//-----------------------------------------------------------------------------
CGFunction::~CGFunction()
{

}
//-----------------------------------------------------------------------------
void CGFunction::CreateOrReplaceFunction(PMetaClassFunction *MetaFunction, QString &ErrorString)
{
	ISQuery qCreateFunction;
	qCreateFunction.SetShowLongQuery(false);
	if (!qCreateFunction.Execute(MetaFunction->GetText()))
	{
		ErrorString = qCreateFunction.GetErrorText();
	}
}
//-----------------------------------------------------------------------------
