#include "CGFunction.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
void CGFunction::CreateOrReplaceFunction(PMetaClassFunction *MetaFunction, QString &ErrorString)
{
	ISQuery qCreateFunction;
	qCreateFunction.SetShowLongQuery(false);
	if (!qCreateFunction.Execute(MetaFunction->Text))
	{
		ErrorString = qCreateFunction.GetErrorText();
	}
}
//-----------------------------------------------------------------------------
