#include "CGSequence.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_SEQUENCES = PREPARE_QUERY("SELECT COUNT(*) "
											"FROM information_schema.sequences t "
											"WHERE t.sequence_catalog = current_database() "
											"AND t.sequence_name = :SequenceName");
//-----------------------------------------------------------------------------
static QString QC_SEQUENCE = "CREATE SEQUENCE public.%1 "
							 "INCREMENT 1 MINVALUE 1 "
							 "MAXVALUE 2147483647 START 1 "
							 "CACHE 1";
//-----------------------------------------------------------------------------
bool CGSequence::CreateSequence(const QString &ClassName)
{
	QString Query = QC_SEQUENCE.arg(GetSequenceNameForClass(ClassName)); //������������ �������
	ISQuery qCreateSequence;
	qCreateSequence.SetShowLongQuery(false);
	return qCreateSequence.Execute(Query);
}
//-----------------------------------------------------------------------------
bool CGSequence::CheckExistSequence(const QString &ClassName, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectSequences(QS_SEQUENCES);
	qSelectSequences.SetShowLongQuery(false);
	qSelectSequences.BindValue(":SequenceName", GetSequenceNameForClass(ClassName));
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
QString CGSequence::GetSequenceNameForClass(const QString &ClassName)
{
	return ClassName + "_sequence";
}
//-----------------------------------------------------------------------------