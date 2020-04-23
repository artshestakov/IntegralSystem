#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISTelephonySummary
{
public:
	static int GetCountIncoming(const ISRangeStruct &RangeStruct); //Получить общее количество входящих звонков по диапазону
	static int GetCountOutcoming(const ISRangeStruct &RangeStruct); //Получить общее количество исходящих звонков по диапазону
	static QString GetDurationAll(const ISRangeStruct &RangeStruct); //Получить общую длительность всех звонков по диапазону
	static QString GetDurationIncoming(const ISRangeStruct &RangeStruct); //Получить общую длительность всех входящих звонков по диапазону
	static QString GetDurationOutcoming(const ISRangeStruct &RangeStruct); //Получить общую длительность всех исходящих звонков по диапазону
	static QString GetAverageIncoming(const ISRangeStruct &RangeStruct); //Получить среднюю длительность входящих звонков по диапазону
	static QString GetAverageOutcoming(const ISRangeStruct &RangeStruct); //Получить среднюю длительность исходящих звонков по диапазону
};
//-----------------------------------------------------------------------------
