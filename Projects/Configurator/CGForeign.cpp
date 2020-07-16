#include "CGForeign.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_FOREIGN = PREPARE_QUERY("SELECT COUNT(*) "
										  "FROM information_schema.constraint_table_usage "
										  "WHERE table_catalog = current_database() "
										  "AND table_schema = current_schema() "
										  "AND constraint_name = :ForeignName");
//-----------------------------------------------------------------------------
static QString QC_FOREIGN = "ALTER TABLE public.%1 "
							"ADD CONSTRAINT %2 FOREIGN KEY (%3) "
							"REFERENCES public.%4(%5_id) "
							"ON DELETE NO ACTION "
							"ON UPDATE NO ACTION "
							"NOT DEFERRABLE;";
//-----------------------------------------------------------------------------
static QString QD_FOREIGN = "ALTER TABLE public.%1 DROP CONSTRAINT %2 RESTRICT; ";
//-----------------------------------------------------------------------------
bool CGForeign::CreateForeign(PMetaForeign *MetaForeign, QString &ErrorString)
{
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(MetaForeign->TableName);
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass);

	QString SqlText;
	SqlText += "ALTER TABLE public." + MetaTable->Name.toLower() + " \n";
	SqlText += "ADD CONSTRAINT " + GetForeignName(MetaForeign) + " FOREIGN KEY (" + MetaTable->Alias + '_' + MetaForeign->Field.toLower() + ") \n";
	SqlText += "REFERENCES public." + MetaTableForeign->Name.toLower() + '(' + MetaTableForeign->Alias.toLower() + '_' + MetaForeign->ForeignField.toLower() + ") \n";
	SqlText += "ON DELETE CASCADE \n";
	SqlText += "ON UPDATE NO ACTION \n";
	SqlText += "NOT DEFERRABLE;";

	ISQuery qCreateForeign;
	qCreateForeign.SetShowLongQuery(false);
	bool Result = qCreateForeign.Execute(SqlText);
	if (!Result)
	{
		ErrorString = qCreateForeign.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGForeign::UpdateForeign(PMetaForeign *MetaForeign, QString &ErrorString)
{
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(MetaForeign->TableName);

	ISQuery qDeleteForeign;
	qDeleteForeign.SetShowLongQuery(false);
	bool Result = qDeleteForeign.Execute(QD_FOREIGN.arg(MetaTable->Name.toLower()).arg(GetForeignName(MetaForeign)));
	if (Result)
	{
		Result = CreateForeign(MetaForeign, ErrorString);
	}
	else
	{
		ErrorString = qDeleteForeign.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGForeign::CheckExistForeign(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString)
{
	ISQuery qSelect(QS_FOREIGN);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":ForeignName", GetForeignName(MetaForeign));
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		Exist = qSelect.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString CGForeign::GetForeignName(PMetaForeign *MetaForeign)
{
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(MetaForeign->TableName);
	return MetaTable->Name.toLower() + '_' + MetaTable->Alias + '_' + MetaForeign->ForeignField.toLower() + "_foreign";
}
//-----------------------------------------------------------------------------
