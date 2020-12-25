#include "ISCore.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISQueryPool.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISDatabase.h"
#include "ISMetaDataHelper.h"
//-----------------------------------------------------------------------------
QString ISCore::GetObjectName(const QString &TableName, int ObjectID)
{
	return GetObjectName(ISMetaData::Instance().GetMetaTable(TableName), ObjectID);
}
//-----------------------------------------------------------------------------
QString ISCore::GetObjectName(PMetaTable *MetaTable, int ObjectID)
{
	if (MetaTable->TitleName.isEmpty()) //Если TitleName у мета-таблицы не заполнен - возвращаем идентификатор объекта
	{
        return QString::number(ObjectID);
	}
	
	QString ObjectName;
	QStringList StringList = MetaTable->TitleName.split(';');
	QString QueryText = "SELECT ";

	if (StringList.count() > 1) //Если имя объекта строится из нескольких полей
	{
		QueryText += "concat(";
		for (const QString &FieldName : StringList) //Обход полей
		{
			PMetaForeign *MetaForeign = MetaTable->GetField(FieldName)->Foreign;
			QueryText += MetaForeign ?
				("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, FieldName) + "), ' ', ") :
				(MetaTable->Alias + '_' + FieldName + ", ' ', ");
		}
		QueryText.chop(7);
		QueryText += ") \n";
	}
	else //Если указано только одно поле
	{
		PMetaForeign *MetaForeign = MetaTable->GetField(MetaTable->TitleName)->Foreign;
		QueryText += MetaForeign ?
			("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, MetaTable->TitleName) + ") \n") :
			(MetaTable->Alias + '_' + MetaTable->TitleName + " \n");
	}

	QueryText += "FROM " + MetaTable->Name + " \n";
	QueryText += "WHERE " + MetaTable->Alias + "_id = :ObjectID";

	ISQuery qSelectName(QueryText);
	qSelectName.BindValue(":ObjectID", ObjectID);
	if (qSelectName.ExecuteFirst())
	{
		QVariant Value = qSelectName.ReadColumn(0);
		switch (Value.type())
		{
		case QVariant::Date: ObjectName = Value.toDate().toString(FORMAT_DATE_V2); break;
		case QVariant::Time: ObjectName = Value.toTime().toString(FORMAT_TIME_V1); break;
		case QVariant::DateTime: ObjectName = Value.toDateTime().toString(FORMAT_DATE_TIME_V2); break;
		default: ObjectName = qSelectName.ReadColumn(0).toString();
		}
	}

	//Удаляем возможные пробелы в конце имени объекта
	ISAlgorithm::RemoveLastSymbolLoop(ObjectName, SYMBOL_SPACE);
	return ObjectName;
}
//-----------------------------------------------------------------------------
void ISCore::PhoneNumberPrepare(QString &PhoneNumber)
{
	for (int i = 0; i < PhoneNumber.size(); ++i) //Обходим строку с номером телефона
	{
		if (!PhoneNumber[i].isDigit()) //Если текущий символ не является цифрой
		{
			PhoneNumber.remove(i, 1); //Удаляем этот символ
			--i; //И уменьшаем индекс
		}
	}
}
//-----------------------------------------------------------------------------
void ISCore::PhoneNumberFormat(QString &PhoneNumber)
{
	PhoneNumber = QString("+7 (%1) %2-%3-%4").
		arg(PhoneNumber.left(3)).
		arg(PhoneNumber.mid(3, 3)).
		arg(PhoneNumber.mid(6, 2)).
		arg(PhoneNumber.right(2));
}
//-----------------------------------------------------------------------------
