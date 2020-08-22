#include "CGSequence.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_SEQUENCES = PREPARE_QUERY("SELECT COUNT(*) "
											"FROM information_schema.sequences "
											"WHERE sequence_catalog = current_database() "
											"AND sequence_name = :SequenceName");
//-----------------------------------------------------------------------------
static QString QC_SEQUENCE = "CREATE SEQUENCE public.%1 "
							 "INCREMENT 1 MINVALUE 1 "
							 "MAXVALUE 2147483647 START 1 "
							 "CACHE 1";
//-----------------------------------------------------------------------------
bool CGSequence::CreateSequence(const QString &TableName, QString &ErrorString)
{
	ISQuery qCreateSequence;
	qCreateSequence.SetShowLongQuery(false);
	bool Result = qCreateSequence.Execute(QC_SEQUENCE.arg(GetSequenceNameForTable(TableName)));
	if (!Result)
	{
		ErrorString = qCreateSequence.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGSequence::CheckExistSequence(const QString &TableName, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectSequences(QS_SEQUENCES);
	qSelectSequences.SetShowLongQuery(false);
	qSelectSequences.BindValue(":SequenceName", GetSequenceNameForTable(TableName));
	bool Result = qSelectSequences.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectSequences.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectSequences.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString CGSequence::GetSequenceNameForTable(const QString &TableName)
{
	return TableName + "_sequence";
}
//-----------------------------------------------------------------------------