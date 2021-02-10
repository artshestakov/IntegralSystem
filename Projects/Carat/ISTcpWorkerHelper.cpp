#include "ISTcpWorkerHelper.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISAlgorithm.h"
#include "ISProperty.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::ConvertDateTimeToString(const QDateTime &DateTime, const QString &TimeFormat)
{
	return DateTime.isValid() ? ConvertDateToString(DateTime.date()) + SYMBOL_SPACE +
		LANG("Carat.In") + SYMBOL_SPACE + DateTime.time().toString(TimeFormat) :
		QString();
}
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::ConvertDateToString(const QDate &Date)
{
	QString Result;
	QDate CurrentDate = QDate::currentDate();
	if (Date == CurrentDate.addDays(-2)) //���������
	{
		Result = LANG("Carat.BeforeYesterday");
	}
	else if (Date == CurrentDate.addDays(-1)) //�����
	{
		Result = LANG("Carat.Yesterday");
	}
	else if (Date == CurrentDate) //�������
	{
		Result = LANG("Carat.Today");
	}
	else if (Date == CurrentDate.addDays(1)) //������
	{
		Result = LANG("Carat.Tomorrow");
	}
	else if (Date == CurrentDate.addDays(2)) //�����������
	{
		Result = LANG("Carat.AfterTomorrow");
	}
	else
	{
		switch (Date.month())
		{
		case 1: Result = LANG("Carat.Month.January"); break;
		case 2: Result = LANG("Carat.Month.February"); break;
		case 3: Result = LANG("Carat.Month.March"); break;
		case 4: Result = LANG("Carat.Month.April"); break;
		case 5: Result = LANG("Carat.Month.May"); break;
		case 6: Result = LANG("Carat.Month.June"); break;
		case 7: Result = LANG("Carat.Month.July"); break;
		case 8: Result = LANG("Carat.Month.August"); break;
		case 9: Result = LANG("Carat.Month.September"); break;
		case 10: Result = LANG("Carat.Month.October"); break;
		case 11: Result = LANG("Carat.Month.November"); break;
		case 12: Result = LANG("Carat.Month.December"); break;
		default:
			break;
		}
		Result = QString::number(Date.day()) + SYMBOL_SPACE +
			Result + SYMBOL_SPACE +
			QString::number(Date.year());
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::GetUptime()
{
	qint64 Seconds = PROPERTY_GET("Uptime").toDateTime().secsTo(QDateTime::currentDateTime()),
		Days = 0;
	while (Seconds > DAY_IN_SECONDS)
	{
		++Days;
		Seconds -= DAY_IN_SECONDS;
	}
	return LANG("Carat.Uptime").arg(Days).arg(QTime(0, 0).addSecs(Seconds).toString(FORMAT_TIME_V3));
}
//-----------------------------------------------------------------------------
QVariant ISTcpWorkerHelper::GetSettingDB(const QString &DBConnectionName, const QString &SettingName)
{
	QVariant Value;
	ISQuery qSelectSettingDB(ISDatabase::Instance().GetDB(DBConnectionName),
		"SELECT sgdb_" + SettingName + " FROM _settingsdatabase WHERE sgdb_uid = :SettingUID");
	qSelectSettingDB.BindValue(":SettingUID", CONST_UID_SETTINGS_DATABASE);
	if (qSelectSettingDB.Execute()) //������ �������� �������
	{
		if (qSelectSettingDB.First()) //��������� �������
		{
			Value = qSelectSettingDB.ReadColumn(0);
		}
		else //����� ��������� � �� ���
		{
			ISLOGGER_E(__CLASS__, "not found setting database. Use default value...");
		}
	}
	else //������ �������
	{
		ISLOGGER_E(__CLASS__, "not getting setting database: " + qSelectSettingDB.GetErrorString());
	}

	//���� �������� ���������� - ���������� ��������� �� ���������
	if (!Value.isValid() || Value.isNull())
	{
		Value = ISMetaData::Instance().GetMetaTable("_SettingsDatabase")->GetField(SettingName)->DefaultValue;
	}
	return Value;
}
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::CreateSqlFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName)
{
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass);
	QString SqlQuery = "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignViewNameField +
		" FROM " + MetaTableForeign->Name +
		" WHERE " + MetaTableForeign->Alias + "_id = " + Alias + '_' + FieldName;
	return SqlQuery;
}
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::CreateSqlFromTable(PMetaTable *MetaTable, QVariantMap &FilterMap, const QVariantList &SearchList, QString SortingField, Qt::SortOrder SortingOrder)
{
	QString SqlText = "SELECT\n",
		SqlTextJoins;
	size_t Index = 0;
	ISStringMap ForeignFields;

	//������� ���� ����-�������
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		//���� ���� ������ � ������� - ���������� ���
		if (MetaField->HideFromList)
		{
			continue;
		}

		if (MetaField->Foreign) //���� �� ���� ���������� ������� ����
		{
			PMetaTable *MetaTableFK = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass);
			QString RandomAlias = MetaTableFK->Alias + QString::number(++Index);
			SqlTextJoins += "LEFT JOIN " + MetaTableFK->Name.toLower() + ' ' + RandomAlias + " ON " + MetaTable->Alias + '.' + MetaTable->Alias + '_' + MetaField->Name.toLower() + " = " + RandomAlias + '.' + MetaTableFK->Alias + '_' + MetaField->Foreign->ForeignField.toLower() + "\n";

			QString Temp;
			QStringList StringList = MetaField->Foreign->ForeignViewNameField.split(';');
			Temp += "concat(";
			for (const QString &FieldName : StringList)
			{
				Temp += RandomAlias + '.' + MetaTableFK->Alias + '_' + FieldName.toLower() + ", ' ',";
			}
			Temp.chop(6);
			Temp += ')';
			SqlText += Temp;
			ForeignFields.emplace(MetaField->Name, Temp);
		}
		else //���� �����������
		{
			if (MetaField->QueryText.isEmpty()) //���� �� �����������
			{
				SqlText += MetaTable->Alias + SYMBOL_POINT + MetaTable->Alias + '_' + MetaField->Name.toLower();
			}
			else //���� �������� �����������
			{
				SqlText += '(' + MetaField->QueryText + ')';
			}
		}
		SqlText += " AS \"" + MetaField->Name + "\",\n";
	}
	SqlText.chop(2);
	SqlTextJoins.chop(1);

	SqlText += "\nFROM " + MetaTable->Name.toLower() + ' ' + MetaTable->Alias + '\n';
	SqlText += SqlTextJoins;

	//���� ���������� ������� - �������������
	if (!FilterMap.isEmpty())
	{
		SqlText += "\nWHERE ";
		for (const auto &MapItem : FilterMap.toStdMap())
		{
			SqlText += MetaTable->Alias + '_' + MapItem.first + " = :" + MapItem.first + "\nAND ";
		}
		SqlText.chop(5);
	}

	//���� ������� ��������� ���������
	bool IsSearch = !SearchList.isEmpty();
	if (IsSearch)
	{
		SqlText += "\nWHERE\n";
		for (const QVariant &Variant : SearchList) //������� ��������� �������
		{
			QVariantMap Map = Variant.toMap();
			QString FieldName = Map["FieldName"].toString();
			ISNamespace::SearchType OperatorType = static_cast<ISNamespace::SearchType>(Map["Operator"].toUInt());
			QVariantList ValueList = Map["Values"].toList();

			SqlText += MetaTable->Alias + '_' + MetaTable->GetField(FieldName)->Name.toLower();
			if (ValueList.size() > 1) //���� �������� ������ ��������� - ���������� �������� "IN"
			{
				switch (OperatorType)
				{
				case ISNamespace::SearchType::Equally: SqlText += " IN("; break;
				case ISNamespace::SearchType::NotEqually: SqlText += " NOT IN("; break;
				default:
					break;
				}
				for (const QVariant &Value : ValueList) //������� ������ ��������
				{
					QString UIDLite = GENERATE_UUID_LITE;
					SqlText += ':' + UIDLite + ',';
					FilterMap[UIDLite] = Value;
				}
				SqlText.chop(1);
				SqlText += ')';
			}
			else //���� �������� ��� ������ - ���������� ������� �������� "="
			{
				QString UIDLite = GENERATE_UUID_LITE;
				switch (OperatorType)
				{
				case ISNamespace::SearchType::Equally: SqlText += " = :" + UIDLite; break;
				case ISNamespace::SearchType::NotEqually: SqlText += " != :" + UIDLite; break;
				case ISNamespace::SearchType::Begins: SqlText += " LIKE :" + UIDLite + " || '%'"; break;
				case ISNamespace::SearchType::Ends: SqlText += " LIKE '%' || :" + UIDLite; break;
				case ISNamespace::SearchType::Contains: SqlText += " LIKE '%' || :" + UIDLite + " || '%'"; break;
				default:
					break;
				}
				FilterMap[UIDLite] = ValueList.front();
			}
			SqlText += "\nAND ";
		}
		SqlText.chop(5);
	}

	//����������� ����������
	PMetaField *MetaFieldSorting = MetaTable->GetField(SortingField);
	bool SortingIsVirtual = !MetaFieldSorting->QueryText.isEmpty(),
		SortingIsForeign = MetaFieldSorting->Foreign;
	if (SortingIsVirtual) //���� ���� �������� ����������� - � �������� ���������� ����� ��������� ������
	{
		SortingField = '(' + MetaFieldSorting->QueryText + ')';
	}
	else //���� �� �����������: ���� �� ���� ���������� ������� ���� - ���������� �������������� ��� - ����� ������ ��� ����
	{
		SortingField = SortingIsForeign ? ForeignFields[MetaFieldSorting->Name] : MetaFieldSorting->Name;
	}

	//��������� ���������� � � �����������
	SqlText += "\nORDER BY " + (SortingIsVirtual || SortingIsForeign ? SortingField :
		MetaTable->Alias + '.' + MetaTable->Alias + '_' + SortingField) + ' ' +
		(SortingOrder == Qt::AscendingOrder ? "ASC" : "DESC");
	return SqlText;
}
//-----------------------------------------------------------------------------
