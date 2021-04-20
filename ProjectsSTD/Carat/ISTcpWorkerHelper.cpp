#include "ISTcpWorkerHelper.h"
#include "ISLocalization.h"
#include "ISAlgorithm.h"
#include "ISProperty.h"
#include "ISConstants.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
std::string ISTcpWorkerHelper::ConvertDateTimeToString(const ISDateTime &DateTime)
{
	return !DateTime.IsNull() ?
        ConvertDateToString(DateTime.Date) + ' ' + LANG("Carat.In") + ' ' + DateTime.Time.ToString() :
        std::string();
}
//-----------------------------------------------------------------------------
std::string ISTcpWorkerHelper::ConvertDateToString(const ISDate &Date)
{
    std::string Result;
	ISDate CurrentDate = ISDate::CurrentDate();
	if (Date.Day == CurrentDate.Day - 2) //���������
	{
		Result = LANG("Carat.BeforeYesterday");
	}
	else if (Date.Day == CurrentDate.Day - 1) //�����
	{
		Result = LANG("Carat.Yesterday");
	}
	else if (Date == CurrentDate) //�������
	{
		Result = LANG("Carat.Today");
	}
	else if (Date.Day == CurrentDate.Day + 1) //������
	{
		Result = LANG("Carat.Tomorrow");
	}
	else if (Date.Day == CurrentDate.Day + 2) //�����������
	{
		Result = LANG("Carat.AfterTomorrow");
	}
	else
	{
		switch (Date.Month)
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
		Result = std::to_string(Date.Day) + ' ' +
			Result + ' ' +
			std::to_string(Date.Year);
	}
	return Result;
}
//-----------------------------------------------------------------------------
std::string ISTcpWorkerHelper::GetUptime()
{
    ISUInt64 Seconds = ISAlgorithm::GetCurrentUnixtime() - ISProperty::Instance().GetUptime(),
        Days = 0;
	while (Seconds > DAY_IN_SECONDS)
	{
		++Days;
        Seconds -= DAY_IN_SECONDS;
	}
    return ISAlgorithm::StringF(LANG("Carat.Uptime").c_str(), Days, ISTime::FromSecond((unsigned int)Seconds).ToString().c_str());
}
//-----------------------------------------------------------------------------
/*QVariant ISTcpWorkerHelper::GetSettingDB(const QString &DBConnectionName, const QString &SettingName)
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
}*/
//-----------------------------------------------------------------------------
std::string ISTcpWorkerHelper::CreateSqlFromTitleName(PMetaForeign *MetaForeign, const std::string &Alias, const std::string &FieldName)
{
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetTable(MetaForeign->ForeignClass);
    return "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignViewNameField +
		" FROM " + MetaTableForeign->Name +
		" WHERE " + MetaTableForeign->Alias + "_id = " + Alias + '_' + FieldName;
}
//-----------------------------------------------------------------------------
std::string ISTcpWorkerHelper::CreateSqlFromTable(PMetaTable *MetaTable, /*QVariantMap &FilterMap, const QVariantList &SearchList, */std::string SortingField, ISNamespace::SortingOrder SortingOrder)
{
    std::string SqlText = "SELECT\n",
		SqlTextJoins;
	size_t Index = 0;
    ISMapString ForeignFields;

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
			PMetaTable *MetaTableFK = ISMetaData::Instance().GetTable(MetaField->Foreign->ForeignClass);
            std::string RandomAlias = MetaTableFK->Alias + std::to_string(++Index);
			SqlTextJoins += "LEFT JOIN " + MetaTableFK->Name + ' ' + RandomAlias + " ON " + MetaTable->Alias + '.' + MetaTable->Alias + '_' + MetaField->Name + " = " + RandomAlias + '.' + MetaTableFK->Alias + '_' + MetaField->Foreign->ForeignField + "\n";

            std::string Temp;
            ISVectorString VectorString = ISAlgorithm::StringSplit(MetaField->Foreign->ForeignViewNameField, ';');
			Temp += "concat(";
			for (const std::string &FieldName : VectorString)
			{
				Temp += RandomAlias + '.' + MetaTableFK->Alias + '_' + FieldName + ", ' ',";
			}
            ISAlgorithm::StringChop(Temp, 6);
			Temp += ')';
			SqlText += Temp;
			ForeignFields.emplace(MetaField->Name, Temp);
		}
		else //���� �����������
		{
			if (MetaField->QueryText.empty()) //���� �� �����������
			{
				SqlText += MetaTable->Alias + '.' + MetaTable->Alias + '_' + MetaField->Name;
			}
			else //���� �������� �����������
			{
				SqlText += '(' + MetaField->QueryText + ')';
			}
		}
		SqlText += " AS \"" + MetaField->Name + "\",\n";
	}
    ISAlgorithm::StringChop(SqlText, 2);
    ISAlgorithm::StringChop(SqlTextJoins, 1);

	SqlText += "\nFROM " + MetaTable->Name + ' ' + MetaTable->Alias + '\n';
	SqlText += SqlTextJoins;

	//���� ���������� ������� - �������������
	/*if (!FilterMap.isEmpty())
	{
		SqlText += "\nWHERE ";
		for (const auto &MapItem : FilterMap.toStdMap())
		{
			SqlText += MetaTable->Alias + '_' + MapItem.first + " = :" + MapItem.first + "\nAND ";
		}
        ISAlgorithm::StringChop(SqlText, 5);
	}*/

	//���� ������� ��������� ���������
	/*bool IsSearch = !SearchList.isEmpty();
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
					QString UIDLite = QString::fromStdString(GENERATE_UUID_LITE());
					SqlText += ':' + UIDLite + ',';
					FilterMap[UIDLite] = Value;
				}
				SqlText.chop(1);
				SqlText += ')';
			}
			else //���� �������� ��� ������ - ���������� ������� �������� "="
			{
				QString UIDLite = QString::fromStdString(GENERATE_UUID_LITE());
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
	}*/

	//����������� ����������
	PMetaField *MetaFieldSorting = MetaTable->GetField(SortingField);
	bool SortingIsVirtual = !MetaFieldSorting->QueryText.empty(),
		SortingIsForeign = MetaFieldSorting->Foreign ? true : false;
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
		(SortingOrder == ISNamespace::SortingOrder::Ascending ? "ASC" : "DESC");
	return SqlText;
}
//-----------------------------------------------------------------------------
