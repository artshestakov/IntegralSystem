#include "CGForeign.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_FOREIGN = PREPARE_QUERY("SELECT COUNT(*) FROM information_schema.constraint_table_usage WHERE table_catalog = :DatabaseName AND table_schema = current_schema() AND constraint_name = :ForeignName");
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
bool CGForeign::CreateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString)
{
	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->TableName);
	PMetaClassTable *MetaTableForeign = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->ForeignClass);

	QString SqlText = QString();
	SqlText += "ALTER TABLE public." + MetaTable->Name.toLower() + " \n";
	SqlText += "ADD CONSTRAINT " + OnGetForeignName(MetaForeign) + " FOREIGN KEY (" + MetaTable->Alias + '_' + MetaForeign->ForeignField.toLower() + ") \n";
	SqlText += "REFERENCES public." + MetaTableForeign->Name.toLower() + '(' + MetaTableForeign->Alias.toLower() + '_' + MetaForeign->ForeignField.toLower() + ") \n";
	SqlText += "ON DELETE CASCADE \n";
	SqlText += "ON UPDATE NO ACTION \n";
	SqlText += "NOT DEFERRABLE;";

	ISQuery qCreateForeign;
	qCreateForeign.SetShowLongQuery(false);
	bool Created = qCreateForeign.Execute(SqlText);
	if (!Created)
	{
		ErrorString = qCreateForeign.GetErrorText();
	}

	return Created;
}
//-----------------------------------------------------------------------------
bool CGForeign::UpdateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString)
{
	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->TableName);

	QString QueryText = QD_FOREIGN;
	QueryText = QueryText.arg(MetaTable->Name.toLower());
	QueryText = QueryText.arg(OnGetForeignName(MetaForeign));

	ISQuery qDeleteForeign;
	qDeleteForeign.SetShowLongQuery(false);
	if (qDeleteForeign.Execute(QueryText))
	{
		bool Created = CreateForeign(MetaForeign, ErrorString);
		return Created;
	}
	else
	{
		ErrorString = qDeleteForeign.GetErrorText();
	}
	
	return false;
}
//-----------------------------------------------------------------------------
bool CGForeign::CheckExistForeign(PMetaClassForeign *MetaForeign)
{
	ISQuery qSelect(QS_FOREIGN);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":DatabaseName", CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE));
	qSelect.BindValue(":ForeignName", OnGetForeignName(MetaForeign));
	if (qSelect.ExecuteFirst())
	{
		int Count = qSelect.ReadColumn("count").toInt();
		if (Count)
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
QString CGForeign::OnGetForeignName(PMetaClassForeign *MetaForeign)
{
	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->TableName);
	return MetaTable->Name.toLower() + '_' + MetaTable->Alias + '_' + MetaForeign->ForeignField.toLower() + "_foreign";
}
//-----------------------------------------------------------------------------
