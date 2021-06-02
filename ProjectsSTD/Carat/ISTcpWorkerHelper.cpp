#include "ISTcpWorkerHelper.h"
#include "ISLocalization.h"
#include "ISAlgorithm.h"
#include "ISProperty.h"
#include "ISConstants.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISLogger.h"
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
	if (Date.Day == CurrentDate.Day - 2) //Позавчера
	{
		Result = LANG("Carat.BeforeYesterday");
	}
	else if (Date.Day == CurrentDate.Day - 1) //Вчера
	{
		Result = LANG("Carat.Yesterday");
	}
	else if (Date == CurrentDate) //Сегодня
	{
		Result = LANG("Carat.Today");
	}
	else if (Date.Day == CurrentDate.Day + 1) //Завтра
	{
		Result = LANG("Carat.Tomorrow");
	}
	else if (Date.Day == CurrentDate.Day + 2) //Послезавтра
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
		Result = ISAlgorithm::StringF("%d %s %d", Date.Day, Result.c_str(), Date.Year);
	}
	return Result;
}
//-----------------------------------------------------------------------------
std::string ISTcpWorkerHelper::GetUptime()
{
    uint64_t Seconds = ISDateTime::GetCurrentUnixtime() - ISProperty::Instance().GetUptime(),
        Days = 0;
	while (Seconds > DAY_IN_SECONDS)
	{
		++Days;
        Seconds -= DAY_IN_SECONDS;
	}
    return ISAlgorithm::StringF(LANG("Carat.Uptime"), Days, ISTime::FromSecond((unsigned int)Seconds).ToString().c_str());
}
//-----------------------------------------------------------------------------
const char* ISTcpWorkerHelper::GetSettingDB(PGconn *DBConnection, const std::string &SettingName)
{
	const char *Value = nullptr;
	ISQuery qSelectSettingDB(DBConnection,
		"SELECT sgdb_" + SettingName + " FROM _settingsdatabase WHERE sgdb_uid = $1");
	qSelectSettingDB.BindUID(CONST_UID_SETTINGS_DATABASE);
	if (qSelectSettingDB.Execute()) //Запрос выполнен успешно
	{
		if (qSelectSettingDB.First()) //Настройка нашлась
		{
			Value = qSelectSettingDB.ReadColumn(0);
		}
		else //Такой настройки в БД нет
		{
			ISLOGGER_E(__CLASS__, "not found setting database. Use default value...");
		}
	}
	else //Ошибка запроса
	{
		ISLOGGER_E(__CLASS__, "not getting setting database: %s", qSelectSettingDB.GetErrorString().c_str());
	}

	//Если значение невалидное - используем указанное по умолчанию
	if (strlen(Value) == 0)
	{
		Value = ISMetaData::Instance().GetTable("_SettingsDatabase")->GetField(SettingName)->DefaultValue.c_str();
	}
	return Value;
}
//-----------------------------------------------------------------------------
std::string ISTcpWorkerHelper::CreateSqlFromTitleName(PMetaForeign *MetaForeign, const std::string &Alias, const std::string &FieldName)
{
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetTable(MetaForeign->ForeignClass);
    return "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignViewNameField +
		" FROM " + MetaTableForeign->Name +
		" WHERE " + MetaTableForeign->Alias + "_id = " + Alias + '_' + FieldName;
}
//-----------------------------------------------------------------------------
std::string ISTcpWorkerHelper::CreateSqlFromTable(PMetaTable *MetaTable, rapidjson::Value &FilterObject, const rapidjson::Value &SearchArray, std::string SortingField, ISNamespace::SortingOrder SortingOrder)
{
    IS_UNUSED(SearchArray);

    std::string SqlText = "SELECT\n",
		SqlTextJoins;
	size_t Index = 0;
    ISMapString ForeignFields;

	//Обходим поля мета-таблицы
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		//Если поле скрыто в таблице - пропускаем его
		if (MetaField->HideFromList)
		{
			continue;
		}

		if (MetaField->Foreign) //Если на поле установлен внешний ключ
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
		else //Поле стандартное
		{
			if (MetaField->QueryText.empty()) //Поле не виртуальное
			{
				SqlText += MetaTable->Alias + '.' + MetaTable->Alias + '_' + MetaField->Name;
			}
			else //Поле является виртуальным
			{
				SqlText += '(' + MetaField->QueryText + ')';
			}
		}
		SqlText += " AS \"" + MetaField->Name + "\",\n";
	}
    ISAlgorithm::StringChop(SqlText, 2);
	SqlText += "\nFROM " + MetaTable->Name + ' ' + MetaTable->Alias;

    if (!SqlTextJoins.empty())
    {
        ISAlgorithm::StringChop(SqlTextJoins, 1);
        SqlText += '\n' + SqlTextJoins;
    }

	//Если фильтрация указана - устанавливаем
	if (FilterObject.MemberCount() > 0)
	{
		SqlText += "\nWHERE ";
        int ParameterNumber = 0;
		for (const auto &MapItem : FilterObject.GetObject())
		{
			SqlText += MetaTable->Alias + '_' + MapItem.name.GetString() + " = $" + std::to_string(++ParameterNumber) + "\nAND ";
		}
        ISAlgorithm::StringChop(SqlText, 5);
	}

	//Если указаны поисковые параметры
	/*bool IsSearch = !SearchList.isEmpty();
	if (IsSearch)
	{
		SqlText += "\nWHERE\n";
		for (const QVariant &Variant : SearchList) //Обходим поисковые условия
		{
			QVariantMap Map = Variant.toMap();
			QString FieldName = Map["FieldName"].toString();
			ISNamespace::SearchType OperatorType = static_cast<ISNamespace::SearchType>(Map["Operator"].toUInt());
			QVariantList ValueList = Map["Values"].toList();

			SqlText += MetaTable->Alias + '_' + MetaTable->GetField(FieldName)->Name.toLower();
			if (ValueList.size() > 1) //Если значений поиска несколько - используем оператор "IN"
			{
				switch (OperatorType)
				{
				case ISNamespace::SearchType::Equally: SqlText += " IN("; break;
				case ISNamespace::SearchType::NotEqually: SqlText += " NOT IN("; break;
				default:
					break;
				}
				for (const QVariant &Value : ValueList) //Обходим список значений
				{
					QString UIDLite = QString::fromStdString(GENERATE_UUID_LITE());
					SqlText += ':' + UIDLite + ',';
					FilterMap[UIDLite] = Value;
				}
				SqlText.chop(1);
				SqlText += ')';
			}
			else //Одно значение для поиска - используем обычный оператор "="
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

	//Анализируем сортировку
	PMetaField *MetaFieldSorting = MetaTable->GetField(SortingField);
	bool SortingIsVirtual = !MetaFieldSorting->QueryText.empty(),
		SortingIsForeign = MetaFieldSorting->Foreign ? true : false;
	if (SortingIsVirtual) //Если поле является виртуальным - в качестве сортировки будет выступать запрос
	{
		SortingField = '(' + MetaFieldSorting->QueryText + ')';
	}
	else //Поле не виртуальное: если на поле установлен внешний ключ - используем сформированное имя - иначе просто имя поля
	{
		SortingField = SortingIsForeign ? ForeignFields[MetaFieldSorting->Name] : MetaFieldSorting->Name;
	}

	//Учитываем сортировку и её направление
	SqlText += "\nORDER BY " + (SortingIsVirtual || SortingIsForeign ? SortingField :
		MetaTable->Alias + '.' + MetaTable->Alias + '_' + SortingField) + ' ' +
		(SortingOrder == ISNamespace::SortingOrder::Ascending ? "ASC" : "DESC");
	return SqlText;
}
//-----------------------------------------------------------------------------
size_t ISTcpWorkerHelper::WriteFunctionINN(void *Pointer, size_t Size, size_t NMemb, std::string *Data)
{
    Data->append((char*)Pointer, Size * NMemb);
    return Size * NMemb;
}
//-----------------------------------------------------------------------------
bool ISTcpWorkerHelper::ParseErrorSQL23505(PMetaTable *MetaTable, const std::string &ErrorDetail, std::string &ErrorString)
{
    //Ищем кавычки
    size_t PosL = ErrorDetail.find('"'),
        PosR = ErrorDetail.rfind('"');

    //Если кавычки слева или справа не найдены - ошибка
    if (PosL == NPOS || PosR == NPOS)
    {
        return false;
    }

    //Вытаскиваем строку из кавычек и делим её пополам
    std::string Temp = ErrorDetail.substr(PosL + 1, PosR - PosL - 1);
    ISVectorString VectorString = ISAlgorithm::StringSplit(Temp, '=');
    if (VectorString.size() != 2)
    {
        return false;
    }

    //Вытаскиваем имена полей
    std::string FieldName = VectorString.front();
    FieldName = FieldName.substr(1, FieldName.size() - 2);
    ISAlgorithm::StringRemoveAllChar(FieldName, ' ');

    size_t FieldCount = 0;
    ISVectorString Fields = ISAlgorithm::StringSplit(FieldName, ',', FieldCount);

    //Обходим поля и получаем локальные имена
    for (size_t i = 0, c = FieldCount; i < c; ++i)
    {
        FieldName = Fields[i];
        PosL = FieldName.find('_');
        if (PosL == NPOS) //Не нашли разделитель
        {
            return false;
        }
        FieldName = FieldName.substr(++PosL); //Убираем разделитель

        //Ищем поле и заменяем обычное имя на локальное
        PMetaField *MetaField = MetaTable->GetField(FieldName);
        if (!MetaField)
        {
            return false;
        }
        Fields[i] = MetaField->LabelName;
    }

    //Вытаскиваем значения полей
    std::string Value = VectorString.back();
    Value = Value.substr(1, Value.size() - 2);

    size_t ValuesCount = 0;
    ISVectorString Values = ISAlgorithm::StringSplit(Value, ',', ValuesCount);

    if (FieldCount != ValuesCount) //Что-то пошло не так
    {
        return false;
    }

    ErrorString.clear(); //Обязательно чистим
    for (size_t i = 0; i < FieldCount; ++i)
    {
        //Удалим возможный пробем вначале
        Temp = Values[i];
        if (Temp.front() == ' ')
        {
            Temp.erase(Temp.begin());
        }
        ErrorString += ISAlgorithm::StringF("%s = %s\n", Fields[i].c_str(), Temp.c_str());
    }
    ISAlgorithm::StringChop(ErrorString, 1);
    ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.RecordAdd.Insert.23505"), ErrorString.c_str());
    return true;
}
//-----------------------------------------------------------------------------
