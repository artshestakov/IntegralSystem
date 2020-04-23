#include "ISTelephonySummary.h"
#include "ISConstants.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_COUNT_RANGE = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM _asteriskcalls "
											  "WHERE ascl_direction = (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = :DirectionUID) "
											  "AND ascl_dialbegin::DATE BETWEEN :BeginDate AND :EndDate");
//-----------------------------------------------------------------------------
static QString QS_DURATION_ALL_RANGE = PREPARE_QUERY("SELECT to_char(((SELECT SUM(ascl_duration) "
													 "FROM _asteriskcalls "
													 "WHERE ascl_duration IS NOT NULL "
													 "AND ascl_dialstatus = (SELECT asds_id FROM _asteriskcallstatus WHERE asds_uid = '{6DA83A49-93D3-4D5F-AFC9-C5C89C82E894}')"
													 "AND ascl_dialbegin::DATE BETWEEN :BeginDate AND :EndDate) || ' second')::INTERVAL, 'HH24:MI:SS')");
//-----------------------------------------------------------------------------
static QString QS_DURATION_RANGE = PREPARE_QUERY("SELECT to_char(((SELECT SUM(ascl_duration) "
												 "FROM _asteriskcalls "
												 "WHERE ascl_duration IS NOT NULL "
												 "AND ascl_direction = (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = :DirectionUID) "
												 "AND ascl_dialstatus = (SELECT asds_id FROM _asteriskcallstatus WHERE asds_uid = '{6DA83A49-93D3-4D5F-AFC9-C5C89C82E894}') "
												 "AND ascl_dialbegin::DATE BETWEEN :BeginDate AND :EndDate) || ' second')::INTERVAL, 'HH24:MI:SS')");
//-----------------------------------------------------------------------------
static QString QS_AVERAGE_RANGE = PREPARE_QUERY("SELECT to_char(((SELECT AVG(ascl_duration)::INTEGER "
												"FROM _asteriskcalls "
												"WHERE ascl_duration IS NOT NULL "
												"AND ascl_direction = (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = :DirectionUID) "
												"AND ascl_dialstatus = (SELECT asds_id FROM _asteriskcallstatus WHERE asds_uid = '{6DA83A49-93D3-4D5F-AFC9-C5C89C82E894}') "
												"AND ascl_dialbegin::DATE BETWEEN :BeginDate AND :EndDate) || ' seconds')::INTERVAL, 'HH24:MI:SS')");
//-----------------------------------------------------------------------------
int ISTelephonySummary::GetCountIncoming(const ISRangeStruct &RangeStruct)
{
	int Result = 0;

	ISQuery qSelectIncoming(QS_COUNT_RANGE);
	qSelectIncoming.BindValue(":DirectionUID", CONST_UID_ASTERISK_DIRECTION_INCOMING);
	qSelectIncoming.BindValue(":BeginDate", RangeStruct.BeginValue);
	qSelectIncoming.BindValue(":EndDate", RangeStruct.EndValue);
	if (qSelectIncoming.ExecuteFirst())
	{
		Result = qSelectIncoming.ReadColumn("count").toInt();
	}

	return Result;
}
//-----------------------------------------------------------------------------
int ISTelephonySummary::GetCountOutcoming(const ISRangeStruct &RangeStruct)
{
	int Result = 0;

	ISQuery qSelectOutcoming(QS_COUNT_RANGE);
	qSelectOutcoming.BindValue(":DirectionUID", CONST_UID_ASTERISK_DIRECTION_OUTGOING);
	qSelectOutcoming.BindValue(":BeginDate", RangeStruct.BeginValue);
	qSelectOutcoming.BindValue(":EndDate", RangeStruct.EndValue);
	if (qSelectOutcoming.ExecuteFirst())
	{
		Result = qSelectOutcoming.ReadColumn("count").toInt();
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISTelephonySummary::GetDurationAll(const ISRangeStruct &RangeStruct)
{
	QString Result;

	ISQuery qSelectDuration(QS_DURATION_ALL_RANGE);
	qSelectDuration.BindValue(":BeginDate", RangeStruct.BeginValue);
	qSelectDuration.BindValue(":EndDate", RangeStruct.EndValue);
	if (qSelectDuration.ExecuteFirst())
	{
		Result = qSelectDuration.ReadColumn("to_char").toString();
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISTelephonySummary::GetDurationIncoming(const ISRangeStruct &RangeStruct)
{
	QString Result;

	ISQuery qSelectDurationIncoming(QS_DURATION_RANGE);
	qSelectDurationIncoming.BindValue(":DirectionUID", CONST_UID_ASTERISK_DIRECTION_INCOMING);
	qSelectDurationIncoming.BindValue(":BeginDate", RangeStruct.BeginValue);
	qSelectDurationIncoming.BindValue(":EndDate", RangeStruct.EndValue);
	if (qSelectDurationIncoming.ExecuteFirst())
	{
		Result = qSelectDurationIncoming.ReadColumn("to_char").toString();
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISTelephonySummary::GetDurationOutcoming(const ISRangeStruct &RangeStruct)
{
	QString Result;

	ISQuery qSelectDurationOutcoming(QS_DURATION_RANGE);
	qSelectDurationOutcoming.BindValue(":DirectionUID", CONST_UID_ASTERISK_DIRECTION_OUTGOING);
	qSelectDurationOutcoming.BindValue(":BeginDate", RangeStruct.BeginValue);
	qSelectDurationOutcoming.BindValue(":EndDate", RangeStruct.EndValue);
	if (qSelectDurationOutcoming.ExecuteFirst())
	{
		Result = qSelectDurationOutcoming.ReadColumn("to_char").toString();
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISTelephonySummary::GetAverageIncoming(const ISRangeStruct &RangeStruct)
{
	QString Result;

	ISQuery qSelectAverage(QS_AVERAGE_RANGE);
	qSelectAverage.BindValue(":DirectionUID", CONST_UID_ASTERISK_DIRECTION_INCOMING);
	qSelectAverage.BindValue(":BeginDate", RangeStruct.BeginValue);
	qSelectAverage.BindValue(":EndDate", RangeStruct.EndValue);
	if (qSelectAverage.ExecuteFirst())
	{
		Result = qSelectAverage.ReadColumn("to_char").toString();
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISTelephonySummary::GetAverageOutcoming(const ISRangeStruct &RangeStruct)
{
	QString Result;

	ISQuery qSelectAverage(QS_AVERAGE_RANGE);
	qSelectAverage.BindValue(":DirectionUID", CONST_UID_ASTERISK_DIRECTION_OUTGOING);
	qSelectAverage.BindValue(":BeginDate", RangeStruct.BeginValue);
	qSelectAverage.BindValue(":EndDate", RangeStruct.EndValue);
	if (qSelectAverage.ExecuteFirst())
	{
		Result = qSelectAverage.ReadColumn("to_char").toString();
	}

	return Result;
}
//-----------------------------------------------------------------------------
