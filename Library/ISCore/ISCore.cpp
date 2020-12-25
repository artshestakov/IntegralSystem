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
	if (MetaTable->TitleName.isEmpty()) //���� TitleName � ����-������� �� �������� - ���������� ������������� �������
	{
        return QString::number(ObjectID);
	}
	
	QString ObjectName;
	QStringList StringList = MetaTable->TitleName.split(';');
	QString QueryText = "SELECT ";

	if (StringList.count() > 1) //���� ��� ������� �������� �� ���������� �����
	{
		QueryText += "concat(";
		for (const QString &FieldName : StringList) //����� �����
		{
			PMetaForeign *MetaForeign = MetaTable->GetField(FieldName)->Foreign;
			QueryText += MetaForeign ?
				("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, FieldName) + "), ' ', ") :
				(MetaTable->Alias + '_' + FieldName + ", ' ', ");
		}
		QueryText.chop(7);
		QueryText += ") \n";
	}
	else //���� ������� ������ ���� ����
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

	//������� ��������� ������� � ����� ����� �������
	ISAlgorithm::RemoveLastSymbolLoop(ObjectName, SYMBOL_SPACE);
	return ObjectName;
}
//-----------------------------------------------------------------------------
void ISCore::PhoneNumberPrepare(QString &PhoneNumber)
{
	for (int i = 0; i < PhoneNumber.size(); ++i) //������� ������ � ������� ��������
	{
		if (!PhoneNumber[i].isDigit()) //���� ������� ������ �� �������� ������
		{
			PhoneNumber.remove(i, 1); //������� ���� ������
			--i; //� ��������� ������
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
