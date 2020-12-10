#include "ISMetaData.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISMetaDataHelper.h"
#include "ISUuid.h"
#include "ISMetaUuidCheckeder.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISMetaData::ISMetaData()
	: ErrorString(NO_ERROR_STRING)
{
	VectorTypes = std::vector<ISMetaDataType>
	{
		{ "Int", ISNamespace::FT_Int, "INTEGER", "ISIntegerEdit", "ISComboSearchNumber", true },
		{ "IntArray", ISNamespace::FT_IntArray, "INTEGER[]", "ISIntegerEdit", "ISComboSearchNumber", true },
		{ "BigInt", ISNamespace::FT_BigInt, "BIGINT", "ISIntegerEdit", "ISComboSearchNumber", true },
		{ "String", ISNamespace::FT_String, "CHARACTER VARYING", "ISLineEdit", "ISComboSearchString", true },
		{ "Text", ISNamespace::FT_Text, "TEXT", "ISTextEdit", "ISComboSearchString", true },
		{ "UID", ISNamespace::FT_UID, "UUID", "ISUuidEdit", "ISComboSearchString", true },
		{ "Bool", ISNamespace::FT_Bool, "BOOLEAN", "ISCheckEdit", "ISComboSearchBase", true },
		{ "Double", ISNamespace::FT_Double, "NUMERIC", "ISDoubleEdit", "ISComboSearchNumber", true },
		{ "Money", ISNamespace::FT_Money, "NUMERIC", "ISMoneyEdit", "ISComboSearchNumber", true },
		{ "ByteArray", ISNamespace::FT_ByteArray, "BYTEA", "ISLineEdit", QString(), false },
		{ "Date", ISNamespace::FT_Date, "DATE", "ISDateEdit", "ISComboSearchNumber", true },
		{ "Time", ISNamespace::FT_Time, "TIME WITHOUT TIME ZONE", "ISTimeEdit", "ISComboSearchNumber", true },
		{ "DateTime", ISNamespace::FT_DateTime, "TIMESTAMP WITHOUT TIME ZONE", "ISDateTimeEdit", "ISComboSearchNumber", true },
		{ "Inn", ISNamespace::FT_Inn, "CHARACTER VARYING", "ISINNEdit", "ISComboSearchString", true },
		{ "Kpp", ISNamespace::FT_Kpp, "CHARACTER VARYING", "ISKPPEdit", "ISComboSearchString", true },
		{ "Ogrn", ISNamespace::FT_Ogrn, "CHARACTER VARYING", "ISOGRNEdit", "ISComboSearchString", true },
		{ "Okpo", ISNamespace::FT_Okpo, "CHARACTER VARYING", "ISOKPOEdit", "ISComboSearchString", true },
		{ "Bik", ISNamespace::FT_Bik, "CHARACTER VARYING", "ISBIKEdit", "ISComboSearchString", true },
		{ "Vin", ISNamespace::FT_Vin, "CHARACTER VARYING", "ISVINEdit", "ISComboSearchString", true },
		{ "Year", ISNamespace::FT_Year, "INTEGER", "ISYearEdit", "ISComboSearchNumber", true },
		{ "Phone", ISNamespace::FT_Phone, "CHARACTER VARYING", "ISPhoneEdit", "ISComboSearchString", true },
		{ "Image", ISNamespace::FT_Image, "BYTEA", "ISImageEdit", QString(), false },
		{ "Color", ISNamespace::FT_Color, "CHARACTER VARYING", "ISColorEdit", QString(), false },
		{ "EMail", ISNamespace::FT_EMail, "CHARACTER VARYING", "ISEMailEdit", "ISComboSearchString", true },
		{ "Volume", ISNamespace::FT_Volume, "INTEGER", "ISVolumeEdit", QString(), false },
		{ "Birthday", ISNamespace::FT_Birthday, "DATE", "ISBirthdayEdit", "ISComboSearchNumber", true },
		{ "Seconds", ISNamespace::FT_Seconds, "INTEGER", "ISIntegerEdit", "ISComboSearchNumber", false },
		{ "Month", ISNamespace::FT_Month, "INTEGER", "ISMonthEdit", "ISComboSearchNumber", false },
		{ "Url", ISNamespace::FT_Url, "CHARACTER VARYING", "ISUrlEdit", "ISComboSearchString", true },
		{ "File", ISNamespace::FT_File, "CHARACTER VARYING", "ISFileEdit", QString(), false },
		{ "TaskImportant", ISNamespace::FT_TaskImportant, "BOOLEAN", "ISTaskImportantEdit", "ISComboSearchBase", true }
	};
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISMetaData::~ISMetaData()
{
	while (!Resources.empty())
	{
		delete ISAlgorithm::VectorTakeBack(Resources);
	}
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISMetaData& ISMetaData::Instance()
{
	static ISMetaData MetaData;
	return MetaData;
}
//-----------------------------------------------------------------------------
QString ISMetaData::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISMetaData::Initialize(const QString &configuration_name, bool InitXSR, bool InitXSF)
{
	bool Result = Initialized;
	if (!Result)
	{
		Result = !configuration_name.isEmpty();
		if (Result)
		{
			ConfigurationName = configuration_name;
			Result = InitializeXSN();
			if (Result)
			{
				if (InitXSR)
				{
					Result = InitializeXSR();
				}

				if (Result)
				{
					if (InitXSF)
					{
						Result = InitializeXSF();
					}
				}
			}

			if (Result)
			{
				Result = CheckUniqueAllIdentifiers(InitXSR);
				if (Result)
				{
					Result = CheckUniqueAllAliases();
				}

				if (Result)
				{
					Result = GenerateSqlFromForeigns();
				}
			}
			Initialized = Result;
		}
		else
		{
			ErrorString = "ConfigurationName is empty.";
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::Initialize(const QVariantList &VariantList)
{
	for (const QVariant &Content : VariantList)
	{
		if (!InitializeXSN(Content.toString()))
		{
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
PMetaTable* ISMetaData::GetMetaTable(const QString &TableName)
{
	PMetaTable *MetaTable = nullptr;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	std::map<QString, PMetaTable *>::const_iterator It = TablesMap.find(TableName);
	if (It != TablesMap.end())
	{
		MetaTable = It->second;
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return MetaTable;
}
//-----------------------------------------------------------------------------
PMetaField* ISMetaData::GetMetaField(PMetaTable *MetaTable, const QString &FieldName)
{
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (MetaField->Name.toLower() == FieldName.toLower())
		{
			return MetaField;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
PMetaField* ISMetaData::GetMetaField(const QString &TableName, const QString &FieldName)
{
	return GetMetaField(TablesMap[TableName], FieldName);
}
//-----------------------------------------------------------------------------
std::vector<PMetaFunction*> ISMetaData::GetFunctions()
{
	return ISAlgorithm::ConvertMapToValues<QString, PMetaFunction *>(FunctionsMap);
}
//-----------------------------------------------------------------------------
std::vector<PMetaTable*> ISMetaData::GetTables()
{
	return ISAlgorithm::ConvertMapToValues<QString, PMetaTable *>(TablesMap);
}
//-----------------------------------------------------------------------------
std::vector<PMetaIndex*> ISMetaData::GetIndexes()
{
	std::vector<PMetaIndex*> Indexes;
	for (PMetaTable *MetaTable : GetTables()) //Обход таблиц
	{
		for (PMetaField *MetaField : MetaTable->Fields) //Обход полей
		{
			if (MetaField->Index)
			{
				Indexes.emplace_back(MetaField->Index);
			}
		}

		//Добавляем составные индексы
		Indexes.insert(Indexes.end(), MetaTable->IndexesCompound.begin(), MetaTable->IndexesCompound.end());
	}
	return Indexes;
}
//-----------------------------------------------------------------------------
std::vector<PMetaForeign*> ISMetaData::GetForeigns()
{
	std::vector<PMetaForeign*> Foreigns;
	for (PMetaTable *MetaTable : GetTables()) //Обход таблиц
	{
		for (PMetaField *MetaField : MetaTable->Fields) //Обход полей
		{
			if (MetaField->Foreign)
			{
				Foreigns.emplace_back(MetaField->Foreign);
			}
		}
	}
	return Foreigns;
}
//-----------------------------------------------------------------------------
std::vector<PMetaResource*> ISMetaData::GetResources()
{
	return Resources;
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckExistTable(const QString &TableName) const
{
	return TablesMap.find(TableName) != TablesMap.end();
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckExitField(PMetaTable *MetaTable, const QString &FieldName) const
{
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (MetaField->Name.toLower() == FieldName.toLower())
		{
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckExistResource(const ISUuid &ResourceUID) const
{
	for (PMetaResource *MetaResource : Resources)
	{
		if (MetaResource->UID == ResourceUID)
		{
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
ISNamespace::FieldType ISMetaData::GetTypeField(const QString &type_name)
{
	for (const ISMetaDataType &Type : VectorTypes)
	{
		if (Type.TypeName == type_name)
		{
			return Type.TypeField;
		}
	}
	IS_ASSERT(false, "Not found field type from string: " + type_name);
	return ISNamespace::FT_Unknown;
}
//-----------------------------------------------------------------------------
QString ISMetaData::GetTypeDB(ISNamespace::FieldType field_type) const
{
	for (const ISMetaDataType &Type : VectorTypes)
	{
		if (Type.TypeField == field_type)
		{
			return Type.TypeDB;
		}
	}
	IS_ASSERT(false, "Not found type db.");
	return QString();
}
//-----------------------------------------------------------------------------
QString ISMetaData::GetTypeDB(const QString &type_name) const
{
	for (const ISMetaDataType &Type : VectorTypes)
	{
		if (Type.TypeName == type_name)
		{
			return Type.TypeDB;
		}
	}
	IS_ASSERT(false, "Not found type db: " + type_name);
	return QString();
}
//-----------------------------------------------------------------------------
QString ISMetaData::GetNameFieldEdit(ISNamespace::FieldType field_type) const
{
	for (const ISMetaDataType &Type : VectorTypes)
	{
		if (Type.TypeField == field_type)
		{
			return Type.ControlWidget;
		}
	}
	IS_ASSERT(false, "Not found control widget from field type.");
	return QString();
}
//-----------------------------------------------------------------------------
QString ISMetaData::GetSearchWidget(ISNamespace::FieldType field_type) const
{
	for (const ISMetaDataType &Type : VectorTypes)
	{
		if (Type.TypeField == field_type)
		{
			return Type.SearchOperatorWidget;
		}
	}
	IS_ASSERT(false, "Not found search operator widget from field type.");
	return QString();
}
//-----------------------------------------------------------------------------
bool ISMetaData::GetSearch(ISNamespace::FieldType field_type) const
{
	for (const ISMetaDataType &Type : VectorTypes)
	{
		if (Type.TypeField == field_type)
		{
			return Type.Search;
		}
	}
	IS_ASSERT(false, "Not found search permission from field type.");
	return false;
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckUniqueAllIdentifiers(bool InitXSR)
{
	QStringList FilterXsnXsr;
	FilterXsnXsr.append("*.xsn");

	if (InitXSR)
	{
		FilterXsnXsr.append("*.xsr");
	}

	QFileInfoList FileInfoList;
	FileInfoList.append(QDir(":Scheme/").entryInfoList(FilterXsnXsr, QDir::Files));

	bool Result = !ConfigurationName.isEmpty();
	if (Result)
	{
		FileInfoList.append(QDir(":_" + ConfigurationName).entryInfoList(FilterXsnXsr, QDir::Files));
	}

	ISMetaUuidCheckeder MetaUuidCheckeder;
	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех файлов мета-данных
	{
		Result = MetaUuidCheckeder.Search(FileInfo);
		if (!Result)
		{
			ErrorString = MetaUuidCheckeder.GetErrorString();
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckUniqueAllAliases()
{
	QStringList StringList;
	bool Result = true;
	for (const auto &MapItem : TablesMap) //Обход таблиц
	{
		PMetaTable *MetaTable = MapItem.second;
		Result = MetaTable ? true : false;
		if (!Result)
		{
			ErrorString = "Error";
			break;
		}

		QString Alias = MetaTable->Alias.toLower();
		Result = !StringList.contains(Alias);
		if (Result)
		{
			StringList.append(Alias);
		}
		else
		{
			ErrorString = QString("Duplicate alias: %1. Table: %2.").arg(Alias).arg(MetaTable->Name);
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::GenerateSqlFromForeigns()
{
	bool Result = true;
	std::vector<PMetaForeign*> Foreigns = GetForeigns();
	for (PMetaForeign *MetaForeign : Foreigns) //Обходим все внешние ключи
	{
		//Внешняя сущность
		PMetaTable *MetaTableExtern = GetMetaTable(MetaForeign->ForeignClass);

		//Проверка наличия сущности, на которую будет ссылаться внешний ключ
		Result = MetaTableExtern ? true : false;
		if (!Result)
		{
			ErrorString = QString("Not found table \"%1\"").arg(MetaForeign->ForeignClass);
			break;
		}

		//Проверка наличия поля во внешней сущности
		Result = MetaTableExtern->ContainsField(MetaForeign->ForeignField);
		if (!Result)
		{
			ErrorString = QString("Not found field \"%1\" in table \"%2\"").arg(MetaForeign->ForeignField).arg(MetaForeign->ForeignClass);
			break;
		}

		//Проверка наличия видимых полей
		QStringList ViewFields = MetaForeign->ForeignViewNameField.split(';');
		for (const QString &ViewField : ViewFields)
		{
			//Если поле не пустое - проверяем. Поле в данном месте может быть пустым,
			//если "ForeignViewNameField" вообще не заполнено ничем (это нормально)
			if (!ViewField.isEmpty())
			{
				Result = MetaTableExtern->ContainsField(ViewField);
				if (!Result)
				{
					ErrorString = QString("Not found field \"%1\" in table \"%2\"").arg(ViewField).arg(MetaForeign->ForeignClass);
					break;
				}
			}
		}

		if (Result)
		{
			MetaForeign->SqlQuery = ISMetaDataHelper::GenerateSqlQueryFromForeign(MetaForeign); //Генерация SQL-запроса для внешнего ключа
		}
		else
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSN()
{
	QStringList Filter("*.xsn");
	QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //Загрузка мета-данных движка

	bool Result = !ConfigurationName.isEmpty();
	if (Result)
	{
		FileInfoList.append(QDir(":_" + ConfigurationName).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка мета-данных конфигурации
	}

	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех XSN файлов
	{
		QFile FileXSN(FileInfo.filePath());
		Result = FileXSN.open(QIODevice::ReadOnly);
		if (Result)
		{
			QString Content = FileXSN.readAll();
			FileXSN.close();
			Result = InitializeXSN(Content);
			if (!Result)
			{
				break;
			}
		}
		else
		{
			ErrorString = FileXSN.errorString();
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSN(const QString &Content)
{
	QDomElement DomElement = ISSystem::GetDomElement(Content);

	QString TagName = DomElement.tagName();
	bool Result = !TagName.isEmpty();
	if (Result)
	{
		Result = TagName == "XSN";
		if (Result)
		{
			CurrentXSN = DomElement.attributes().namedItem("Name").nodeValue();
			Result = !CurrentXSN.isEmpty();
			if (Result)
			{
				QDomNode DomNode = DomElement.firstChild();
				while (!DomNode.isNull())
				{
					QDomNode CurrentNode = DomNode;
					Result = CurrentNode.nodeName() == "Table";
					if (Result)
					{
						Result = InitializeXSNTable(CurrentNode);
						if (!Result)
						{
							break;
						}
					}
					else
					{
						ErrorString = QString("Invalid NodeName \"%1\" in XSN file %1").arg(CurrentNode.nodeName()).arg(CurrentXSN);
						break;
					}
					DomNode = DomNode.nextSibling();
				}
				CurrentXSN.clear();
			}
			else
			{
				ErrorString = "Empty schema name";
			}
		}
		else
		{
			ErrorString = QString("Invalid tag name \"%1\" in XSN file.").arg(TagName);
		}
	}
	else
	{
		ErrorString = "Empty tag name";
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTable(QDomNode &DomNode)
{
	bool Result = !DomNode.attributes().isEmpty();
	if (Result)
	{
		PMetaTable *MetaTable = new PMetaTable();
		QString TableName = DomNode.attributes().namedItem("Name").nodeValue();
		QString Parent = DomNode.attributes().namedItem("Parent").nodeValue();

		Result = !TableName.isEmpty();
		if (Result)
		{
			Result = !TableName.contains(SYMBOL_SPACE);
			if (Result)
			{
				QDomNamedNodeMap DomNamedNodeMap = DomNode.attributes();
				MetaTable->Name = DomNamedNodeMap.namedItem("Name").nodeValue();
				MetaTable->UID = DomNamedNodeMap.namedItem("UID").nodeValue();
				MetaTable->Alias = DomNamedNodeMap.namedItem("Alias").nodeValue();
				MetaTable->LocalName = DomNamedNodeMap.namedItem("LocalName").nodeValue();
				MetaTable->LocalListName = DomNamedNodeMap.namedItem("LocalListName").nodeValue();
				MetaTable->TitleName = DomNamedNodeMap.namedItem("TitleName").nodeValue();
				MetaTable->UseRoles = QVariant(DomNamedNodeMap.namedItem("UseRoles").nodeValue()).toBool();
				MetaTable->ClassFilter = DomNamedNodeMap.namedItem("ClassFilter").nodeValue();
				MetaTable->ObjectForm = DomNamedNodeMap.namedItem("ObjectForm").nodeValue();
				MetaTable->ShowOnly = QVariant(DomNamedNodeMap.namedItem("ShowOnly").nodeValue()).toBool();
				MetaTable->IsSystem = QVariant(DomNamedNodeMap.namedItem("IsSystem").nodeValue()).toBool();

				if (Parent.isEmpty()) //Если мета-таблица не является запросом, проинициализировать системные поля
				{
					InitializeXSNTableSystemFields(MetaTable); //Инициализация системных полей
				}

				Result = InitializeXSNTableSystemFieldsVisible(MetaTable, GetChildDomNode(DomNode, "SystemFields").firstChild()); //Инициализация видимости системных полей
				if (Result)
				{
					Result = InitializeXSNTableFields(MetaTable, GetChildDomNode(DomNode, "Fields").firstChild()); //Инициализация пользовательских полей
				}

				if (Result)
				{
					Result = InitializeXSNTableIndexes(MetaTable, GetChildDomNode(DomNode, "Indexes").firstChild()); //Инициализация индексов
				}

				if (Result)
				{
					Result = InitializeXSNTableForeigns(MetaTable, GetChildDomNode(DomNode, "Foreigns").firstChild()); //Инициализация внешних ключей
				}

				if (Result)
				{
					Result = InitializeXSNTableEscorts(MetaTable, GetChildDomNode(DomNode, "Escorts").firstChild()); //Инициализация эскортов
				}

				if (Result)
				{
					//Проверка на заполнение обязательных атрибутов
					Result = !MetaTable->Name.isEmpty();
					if (Result)
					{
						Result = !MetaTable->UID.isEmpty();
						if (Result)
						{
							Result = !MetaTable->Alias.isEmpty();
							if (Result)
							{
								Result = !MetaTable->LocalName.isEmpty();
								if (Result)
								{
									Result = !MetaTable->LocalListName.isEmpty();
									if (Result)
									{
										QStringList TitleFields = MetaTable->TitleName.split(';');
										for (const QString &FieldName : TitleFields)
										{
											Result = MetaTable->ContainsField(FieldName);
											if (!Result)
											{
												ErrorString = QString("Invalid field name \"%1\" in title name. Table name: %2").arg(MetaTable->TitleName).arg(TableName);
												break;
											}
										}

										if (Result)
										{
											Result = !TablesMap.count(TableName);
											if (Result)
											{
												TablesMap.emplace(TableName, MetaTable);
											}
											else
											{
												ErrorString = QString("Table \"%1\" already exist in meta data").arg(TableName);
											}
										}
									}
									else
									{
										ErrorString = QString("Empty table local list name \"%1\".").arg(MetaTable->Name);
									}
								}
								else
								{
									ErrorString = QString("Empty table local name \"%1\".").arg(MetaTable->Name);
								}
							}
							else
							{
								ErrorString = QString("Empty table alias \"%1\".").arg(MetaTable->Name);
							}
						}
						else
						{
							ErrorString = QString("Empty uid table \"%1\".").arg(MetaTable->Name);
						}
					}
					else
					{
						ErrorString = "Empty table name.";
					}
				}
			}
			else
			{
				ErrorString = QString("Forbidden symbol ' ' in table name: %1").arg(TableName);
			}
		}
		else
		{
			ErrorString = QString("Empty table name. File: %1. Line: %2").arg(CurrentXSN).arg(DomNode.lineNumber());
		}
	}
	else
	{
		ErrorString = QString("Empty attributes table. File: %1. Line: %2").arg(CurrentXSN).arg(DomNode.lineNumber());
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSNTableSystemFields(PMetaTable *MetaTable)
{
	QFile File(PATH_SCHEMA_TEMPLATE_FIELDS);
	IS_ASSERT(File.open(QIODevice::ReadOnly), File.errorString());
	QString Content = File.readAll();
	File.close();

	QDomElement DomElement = ISSystem::GetDomElement(Content);
	QDomNode DomNode = DomElement.firstChild();
	InitializeXSNTableFields(MetaTable, DomNode);

	PMetaField *FieldID = MetaTable->GetField("ID");
	FieldID->Index = new PMetaIndex(true, MetaTable->Alias, MetaTable->Name, FieldID->Name);

	PMetaField *FieldUID = MetaTable->GetField("UID");
	FieldUID->Index = new PMetaIndex(true, MetaTable->Alias, MetaTable->Name, FieldUID->Name);
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableSystemFieldsVisible(PMetaTable *MetaTable, const QDomNode &DomNode)
{
	bool Result = true;
	QDomNode Temp = DomNode;
	while (!Temp.isNull()) //Обход полей
	{
		Result = !Temp.attributes().isEmpty();
		if (!Result)
		{
			ErrorString = QString("Empty attributes system field. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
			break;
		}

		QString FieldName = Temp.attributes().namedItem("Name").nodeValue();
		Result = !FieldName.isEmpty();
		if (!Result)
		{
			ErrorString = QString("Empty system field name. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
			break;
		}

		PMetaField *MetaField = MetaTable->GetField(FieldName);
		Result = MetaField ? true : false;
		if (!Result)
		{
			ErrorString = QString("Not found visible system field %1. File: %2. Line: %3").arg(FieldName).arg(CurrentXSN).arg(Temp.lineNumber());
			break;
		}

		QString LocalName = Temp.attributes().namedItem("LocalName").nodeValue();
		if (!LocalName.isEmpty()) //Если локальное имя переопределено
		{
			MetaField->LabelName = LocalName;
			MetaField->LocalListName = LocalName;
		}

		QString AsType = Temp.attributes().namedItem("AsType").nodeValue();
		if (!AsType.isEmpty()) //Переопределен тип
		{
			MetaField->Type = GetTypeField(AsType);
		}
		MetaTable->SystemFieldsVisible.emplace_back(MetaField);
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableFields(PMetaTable *MetaTable, const QDomNode &DomNode)
{
	bool Result = false;
	QDomNode Temp = DomNode;
	while (!Temp.isNull()) //Обход полей
	{
		if (!Temp.isComment()) //Если поле не закоментировано - обрабатываем его
		{
			Result = !Temp.attributes().isEmpty();
			if (!Result)
			{
				ErrorString = QString("Empty attributes field. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
				break;
			}

			Result = Temp.nodeName() == "Field";
			if (!Result)
			{
				ErrorString = QString("Invalid field tag name: %1").arg(Temp.nodeName());
				break;
			}

			QString FieldName = Temp.attributes().namedItem("Name").nodeValue();

			Result = !FieldName.isEmpty();
			if (!Result)
			{
				ErrorString = QString("Empty field name. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
				break;
			}

			Result = !MetaTable->GetField(FieldName);
			if (!Result)
			{
				ErrorString = QString("Field \"%1\" already exist. TableName: %2").arg(FieldName).arg(MetaTable->Name);
				break;
			}

			QDomNamedNodeMap DomNamedNodeMap = Temp.attributes();
			PMetaField *MetaField = new PMetaField();
			MetaField->UID = DomNamedNodeMap.namedItem("UID").nodeValue();
			MetaField->Name = FieldName;
			MetaField->Type = ISMetaData::Instance().GetTypeField(DomNamedNodeMap.namedItem("Type").nodeValue());
			MetaField->Size = DomNamedNodeMap.namedItem("Size").nodeValue().toInt();
			MetaField->Upper = QVariant(DomNamedNodeMap.namedItem("Upper").nodeValue()).toBool();
			MetaField->Lower = QVariant(DomNamedNodeMap.namedItem("Lover").nodeValue()).toBool();
			MetaField->DefaultValue = DomNamedNodeMap.namedItem("DefaultValue").nodeValue().isEmpty() ? QVariant() : DomNamedNodeMap.namedItem("DefaultValue").nodeValue();
			MetaField->DefaultValueWidget = DomNamedNodeMap.namedItem("DefaultValueWidget").nodeValue().isEmpty() ? QVariant() : DomNamedNodeMap.namedItem("DefaultValueWidget").nodeValue();
			MetaField->LabelName = DomNamedNodeMap.namedItem("LabelName").nodeValue().isEmpty() ? MetaField->Name : DomNamedNodeMap.namedItem("LabelName").nodeValue();
			MetaField->LocalListName = DomNamedNodeMap.namedItem("LocalListName").nodeValue().isEmpty() ? MetaField->Name : DomNamedNodeMap.namedItem("LocalListName").nodeValue();
			MetaField->NotNull = QVariant(DomNamedNodeMap.namedItem("NotNull").nodeValue()).toBool();
			MetaField->ReadOnly = QVariant(DomNamedNodeMap.namedItem("ReadOnly").nodeValue()).toBool();
			MetaField->HideFromObject = QVariant(DomNamedNodeMap.namedItem("HideFromObject").nodeValue()).toBool();
			MetaField->HideFromList = QVariant(DomNamedNodeMap.namedItem("HideFromList").nodeValue()).toBool();
			MetaField->NotSearch = QVariant(DomNamedNodeMap.namedItem("NotSearch").nodeValue()).toBool();
			MetaField->Hint = DomNamedNodeMap.namedItem("Hint").nodeValue();
			MetaField->PlaceholderText = DomNamedNodeMap.namedItem("PlaceholderText").nodeValue();
			MetaField->ControlWidget = DomNamedNodeMap.namedItem("ControlWidget").nodeValue();
			MetaField->RegExp = DomNamedNodeMap.namedItem("RegExp").nodeValue();
			MetaField->IsSystem = QVariant(DomNamedNodeMap.namedItem("IsSystem").nodeValue()).toBool();
			MetaField->QueryText = DomNamedNodeMap.namedItem("QueryText").nodeValue();
			MetaField->Sequence = QVariant(DomNamedNodeMap.namedItem("Sequence").nodeValue()).toBool();
			MetaField->PrimaryKey = QVariant(DomNamedNodeMap.namedItem("PrimaryKey").nodeValue()).toBool();
			MetaField->LayoutName = DomNamedNodeMap.namedItem("LayoutName").nodeValue();
			MetaField->SeparatorName = DomNamedNodeMap.namedItem("SeparatorName").nodeValue();
			MetaTable->Fields.emplace_back(MetaField);

			if (MetaField->QueryText.isEmpty())
			{
				//Проверка на заполнение обязательных атрибутов
				Result = !MetaField->UID.isEmpty();
				if (!Result)
				{
					ErrorString = QString("Empty field uid.");
					break;
				}

				Result = !MetaField->Name.isEmpty();
				if (!Result)
				{
					ErrorString = QString("Empty field name.");
					break;
				}

				Result = !MetaField->LabelName.isEmpty();
				if (!Result)
				{
					ErrorString = QString("Empty field \"%1\" label name.").arg(MetaField->UID);
					break;
				}

				Result = !MetaField->LocalListName.isEmpty();
				if (!Result)
				{
					ErrorString = QString("Empty field \"%1\" local list name.").arg(MetaField->UID);
					break;
				}
			}
		}
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableIndexes(PMetaTable *MetaTable, const QDomNode &DomNode)
{
	bool Result = true;
	QDomNode Temp = DomNode;
	while (!Temp.isNull()) //Обход индексов
	{
		if (!Temp.isComment()) //Если обнаружен комментарий - переходить на следующий НОД
		{
			Result = !Temp.attributes().isEmpty();
			if (!Result)
			{
				ErrorString = QString("Empty attributes index. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
				break;
			}

			Result = Temp.nodeName() == "Index";
			if (!Result)
			{
				ErrorString = QString("Invalid index tag name: %1").arg(Temp.nodeName());
				break;
			}

			QString FieldName = Temp.attributes().namedItem("Field").nodeValue();
			Result = !FieldName.isEmpty();
			if (!Result)
			{
				ErrorString = QString("Empty index name. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
				break;
			}

			bool Unique = QVariant(Temp.attributes().namedItem("Unique").nodeValue()).toBool();
			QStringList IndexList = FieldName.split(';');
			if (IndexList.count() > 1) //Если индекс составной
			{
				PMetaIndex *Index = new PMetaIndex(Unique, MetaTable->Alias, MetaTable->Name, QString());
				for (const QString &IndexName : IndexList)
				{
					Index->Fields.emplace_back(IndexName);
				}
				MetaTable->IndexesCompound.emplace_back(Index);
			}
			else //Индекс стандартный
			{
				PMetaField *MetaField = MetaTable->GetField(FieldName);
				Result = MetaField ? true : false;
				if (!Result)
				{
					ErrorString = QString("Not found field from name: %1. TableName: %2").arg(FieldName).arg(MetaTable->Name);
					break;
				}

				Result = !MetaField->Index;
				if (!Result)
				{
					ErrorString = QString("Index already exist. TableName: %1. FieldName: %2.").arg(MetaTable->Name).arg(FieldName);
					break;
				}
				MetaField->Index = new PMetaIndex(Unique, MetaTable->Alias, MetaTable->Name, FieldName);
			}
		}
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableForeigns(PMetaTable *MetaTable, const QDomNode &DomNode)
{
	bool Result = true;
	QDomNode Temp = DomNode;
	while (!Temp.isNull())
	{
		if (!Temp.isComment()) //Если обнаружен комментарий - переходить на следующий НОД
		{
			Result = !Temp.attributes().isEmpty();
			if (!Result)
			{
				ErrorString = QString("Empty attributes foreign. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
				break;
			}

			Result = Temp.nodeName() == "Foreign";
			if (!Result)
			{
				ErrorString = QString("Invalid foreign tag name: %1").arg(Temp.nodeName());
				break;
			}

			QString FieldName = Temp.attributes().namedItem("Field").nodeValue();
			Result = !FieldName.isEmpty();
			if (!Result)
			{
				ErrorString = QString("Empty foreign field. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
				break;
			}

			//Проверка наличия поля - на котором делается внешний ключ
			PMetaField *MetaField = MetaTable->GetField(FieldName);
			Result = MetaField ? true : false;
			if (!Result)
			{
				ErrorString = QString("Not found field \"%1\" in table \"%2\"").arg(FieldName).arg(MetaTable->Name);
				break;
			}

			//Проверка наличия внешнего ключа на этом поле
			Result = !MetaField->Foreign;
			if (!Result)
			{
				ErrorString = QString("Foreign already exist. TableName: %1. FieldName: %2.").arg(MetaTable->Name).arg(FieldName);
				break;
			}

			QDomNamedNodeMap DomNamedNodeMap = Temp.attributes();
			PMetaForeign *MetaForeign = new PMetaForeign(
				FieldName,
				DomNamedNodeMap.namedItem("ForeignClass").nodeValue(),
				DomNamedNodeMap.namedItem("ForeignField").nodeValue(),
				DomNamedNodeMap.namedItem("ForeignViewNameField").nodeValue(),
				DomNamedNodeMap.namedItem("OrderField").nodeValue(),
				MetaTable->Name);
			MetaField->Foreign = MetaForeign;
		}
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableEscorts(PMetaTable *MetaTable, const QDomNode &DomNode)
{
	bool Result = true;
	QDomNode Temp = DomNode;
	while (!Temp.isNull())
	{
		IS_ASSERT(Temp.attributes().length(), QString("Empty attributes escort. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));
		Result = !Temp.attributes().isEmpty();
		if (!Result)
		{
			ErrorString = QString("Empty attributes escort. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
			break;
		}

		QDomNamedNodeMap DomNamedNodeMap = Temp.attributes();
		PMetaEscort *MetaEscort = new PMetaEscort();
		MetaEscort->LocalName = DomNamedNodeMap.namedItem("LocalName").nodeValue();
		MetaEscort->TableName = DomNamedNodeMap.namedItem("TableName").nodeValue();
		MetaEscort->FilterField = DomNamedNodeMap.namedItem("FilterField").nodeValue();
		MetaEscort->ClassName = DomNamedNodeMap.namedItem("ClassName").nodeValue();
		MetaTable->Escorts.emplace_back(MetaEscort);
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSR()
{
	QStringList Filter("*.xsr");
	QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //Загрузка ресурсов движка

	bool Result = !ConfigurationName.isEmpty();
	if (Result)
	{
		FileInfoList.append(QDir(":_" + ConfigurationName).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка ресурсов конфигурации
	}

	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех XSR файлов
	{
		QFile FileXSR(FileInfo.filePath());
		Result = FileXSR.open(QIODevice::ReadOnly);
		if (Result)
		{
			QString Content = FileXSR.readAll();
			FileXSR.close();
			Result = InitializeXSR(Content);
		}
		else
		{
			ErrorString = QString("Not opened xsr file: %1").arg(FileInfo.filePath());
		}

		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSR(const QString &Content)
{
	QDomElement DomElement = ISSystem::GetDomElement(Content);
	QString TagName = DomElement.tagName();
	bool Result = !TagName.isEmpty();
	if (Result)
	{
		Result = TagName == "XSR";
		if (Result)
		{
			QString SchemeName = DomElement.attributes().namedItem("Name").nodeValue();
			Result = !SchemeName.isEmpty();
			if (Result)
			{
				CurrentXSR = SchemeName;
				QDomNode DomNode = DomElement.firstChild();
				while (!DomNode.isNull()) //Обход ресурсов
				{
					if (!DomNode.isComment())
					{
						QDomNamedNodeMap DomNamedNodeMap = DomNode.attributes();
						Result = !DomNamedNodeMap.isEmpty();
						if (!Result)
						{
							ErrorString = QString("Empty resource arguments. File: %1. Line: %2").arg(CurrentXSR).arg(DomNode.lineNumber());
							break;
						}

						QString TableName = DomNode.nodeName();
						Result = !TableName.isEmpty();
						if (!Result)
						{
							ErrorString = QString("Empty resource name. File: %1. Line: %2").arg(CurrentXSR).arg(DomNode.lineNumber());
							break;
						}

						PMetaResource *MetaResource = new PMetaResource();
						MetaResource->TableName = TableName;
						MetaResource->UID = DomNamedNodeMap.namedItem("UID").nodeValue();
						DomNamedNodeMap.removeNamedItem("UID");

						for (int i = 0, c = DomNamedNodeMap.size(); i < c; ++i) //Обход полей ресурса
						{
							QDomNode DomItem = DomNamedNodeMap.item(i); // Поле-значение
							QString FieldName = DomItem.nodeName(); //Имя поля
							QString Value = DomItem.nodeValue(); //Значение поля

							PMetaTable *MetaTable = TablesMap[TableName];
							Result = MetaTable ? true : false;
							if (!Result)
							{
								ErrorString = QString("Invalid table name \"%1\" in resource. File: %2. Line: %3").arg(TableName).arg(CurrentXSR).arg(DomNode.lineNumber());
								break;
							}

							PMetaField *MetaField = MetaTable->GetField(FieldName);
							Result = MetaField ? true : false;
							if (Result)
							{
								MetaResource->AddField(FieldName, Value);
							}
							else
							{
								ErrorString = QString("Field \"%1\" in resource not exist. File: %2. Line: %3").arg(FieldName).arg(CurrentXSR).arg(DomNode.lineNumber());
								break;
							}
						}

						if (Result)
						{
							Result = !MetaResource->UID.isEmpty();
							if (Result)
							{
								Resources.emplace_back(MetaResource);
							}
							else
							{
								ErrorString = QString("Empty UID in resource. File: %1. Line: %2").arg(CurrentXSR).arg(DomNode.lineNumber());
								break;
							}
						}
						else
						{
							break;
						}
					}
					DomNode = DomNode.nextSibling();
				}
				CurrentXSR.clear();
			}
			else
			{
				ErrorString = "Empty schema name";
			}
		}
		else
		{
			ErrorString = QString("Invalid tag name \"%1\" in XSR file").arg(TagName);
		}
	}
	else
	{
		ErrorString = "Empty tag name";
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSF()
{
	QStringList Filter("*.xsf");
	QFileInfoList FileInfoList;
	FileInfoList.append(QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка функций движка

	bool Result = !ConfigurationName.isEmpty();
	if (Result)
	{
		FileInfoList.append(QDir(":_" + ConfigurationName).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка функций конфигурации
	}

	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех XSF файлов
	{
		QFile FileXSF(FileInfo.filePath());
		Result = FileXSF.open(QIODevice::ReadOnly);
		if (Result)
		{
			QString Content = FileXSF.readAll();
			FileXSF.close();
			Result = InitializeXSF(Content);
		}
		else
		{
			ErrorString = QString("Not opened xsf file: %1").arg(FileInfo.filePath());
		}

		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSF(const QString &Content)
{
	QDomElement DomElement = ISSystem::GetDomElement(Content);
	QString TagName = DomElement.tagName();
	bool Result = !TagName.isEmpty();
	if (Result)
	{
		Result = TagName == "XSF";
		if (Result)
		{
			QString SchemeName = DomElement.attributes().namedItem("Name").nodeValue();
			Result = !SchemeName.isEmpty();
			if (Result)
			{
				CurrentXSF = SchemeName;
				QDomNode DomNode = DomElement.firstChild();
				while (!DomNode.isNull()) //Обход элементов
				{
					QDomNamedNodeMap DomNamedNodeMap = DomNode.attributes();
					Result = !DomNamedNodeMap.isEmpty();
					if (!Result)
					{
						ErrorString = QString("Empty element arguments. File: %1. Line: %2").arg(CurrentXSF).arg(DomNode.lineNumber());
						break;
					}

					QString ElementName = DomNode.nodeName();
					Result = !ElementName.isEmpty();
					if (!Result)
					{
						ErrorString = QString("Empty element name. File: %1. Line: %2").arg(CurrentXSR).arg(DomNode.lineNumber());
						break;
					}

					if (ElementName == "Function")
					{
						PMetaFunction *MetaFunction = new PMetaFunction();
						MetaFunction->Name = DomNode.attributes().namedItem("Name").nodeValue();
						MetaFunction->Comment = DomNode.attributes().namedItem("Comment").nodeValue();
						MetaFunction->Text = DomNode.toElement().text();
						FunctionsMap.emplace(MetaFunction->Name, MetaFunction);
					}
					DomNode = DomNode.nextSibling();
				}
				CurrentXSF.clear();
			}
			else
			{
				ErrorString = "Empty schema name";
			}
		}
		else
		{
			ErrorString = QString("Invalid tag name \"%1\" in XSF file.").arg(TagName);
		}
	}
	else
	{
		ErrorString = "Empty tag name";
	}
	return Result;
}
//-----------------------------------------------------------------------------
QDomNode ISMetaData::GetChildDomNode(QDomNode &TableNode, const QString &TagName) const
{
	QDomNode DomNode = TableNode.firstChild();
	while (!DomNode.isNull())
	{
		if (DomNode.nodeName() == TagName)
		{
			return DomNode;
		}
		DomNode = DomNode.nextSibling();
	}
	return QDomNode();
}
//-----------------------------------------------------------------------------
