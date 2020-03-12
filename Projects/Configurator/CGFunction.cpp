#include "CGFunction.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
bool CGFunction::CreateOrReplaceFunction(PMetaFunction *MetaFunction, QString &ErrorString)
{
	ISQuery qCreateFunction;
	qCreateFunction.SetShowLongQuery(false);
	bool Result = qCreateFunction.Execute(MetaFunction->Text);
	if (!Result)
	{
		ErrorString = qCreateFunction.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
