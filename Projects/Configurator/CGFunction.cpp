#include "CGFunction.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_DEPENDECIES = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM information_schema.columns "
											  "WHERE table_catalog = current_database() "
											  "AND table_schema = current_schema() "
											  "AND column_default = :FunctionName");
//-----------------------------------------------------------------------------
static QString QS_FUNCTION = PREPARE_QUERY("SELECT COUNT(*) "
										   "FROM information_schema.routines "
										   "WHERE routine_catalog = current_database() "
										   "AND routine_schema = current_schema() "
										   "AND routine_name = :FunctionName "
										   "AND routine_type = 'FUNCTION'");
//-----------------------------------------------------------------------------
bool CGFunction::CheckDependencies(PMetaClassFunction *MetaFunction)
{
	ISQuery qSelect(QS_DEPENDECIES);
	qSelect.BindValue(":FunctionName", QString("%1()").arg(MetaFunction->Name));
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("count").toBool();
	}
	return false;
}
//-----------------------------------------------------------------------------
bool CGFunction::CheckExist(PMetaClassFunction *MetaFunction, QString &ErrorString)
{
	ISQuery qSelect(QS_FUNCTION);
	qSelect.BindValue(":FunctionName", MetaFunction->Name);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("count").toBool();
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return false;
}
//-----------------------------------------------------------------------------
void CGFunction::CreateOrReplaceFunction(PMetaClassFunction *MetaFunction, QString &ErrorString)
{
	ISQuery qCreateFunction;
	qCreateFunction.SetShowLongQuery(false);
	if (!qCreateFunction.Execute(MetaFunction->Text))
	{
		ErrorString = qCreateFunction.GetErrorString();
	}
}
//-----------------------------------------------------------------------------
void CGFunction::Delete(PMetaClassFunction *MetaFunction)
{
	ISQuery().Execute("DROP FUNCTION public." + MetaFunction->Name);
}
//-----------------------------------------------------------------------------
