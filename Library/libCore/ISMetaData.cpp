#include "ISMetaData.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
#include "ISLogger.h"
#include "ISMetaDataHelper.h"
#include "ISUuid.h"
#include "ISMetaUuidCheckeder.h"
#include "ISMetaViewQuery.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISMetaData::ISMetaData()
	: ErrorString(NO_ERROR_STRING)
{
	AssociationTypes.Insert("Int", ISNamespace::FT_Int, "INTEGER", "ISIntegerEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("String", ISNamespace::FT_String, "CHARACTER VARYING", "ISLineEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Text", ISNamespace::FT_Text, "TEXT", "ISTextEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("UID", ISNamespace::FT_UID, "UUID", "ISUuidEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Bool", ISNamespace::FT_Bool, "BOOLEAN", "ISCheckEdit", "ISComboSearchBase", true);
	AssociationTypes.Insert("Double", ISNamespace::FT_Double, "NUMERIC", "ISDoubleEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("ByteArray", ISNamespace::FT_ByteArray, "BYTEA", QString(), QString(), false);
	AssociationTypes.Insert("Date", ISNamespace::FT_Date, "DATE", "ISDateEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("Time", ISNamespace::FT_Time, "TIME WITHOUT TIME ZONE", "ISTimeEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("DateTime", ISNamespace::FT_DateTime, "TIMESTAMP WITHOUT TIME ZONE", "ISDateTimeEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("Inn", ISNamespace::FT_Inn, "CHARACTER VARYING", "ISINNEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Kpp", ISNamespace::FT_Kpp, "CHARACTER VARYING", "ISKPPEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Ogrn", ISNamespace::FT_Ogrn, "CHARACTER VARYING", "ISOGRNEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Okpo", ISNamespace::FT_Okpo, "CHARACTER VARYING", "ISOKPOEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Bik", ISNamespace::FT_Bik, "CHARACTER VARYING", "ISBIKEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Icq", ISNamespace::FT_Icq, "CHARACTER VARYING", "ISICQEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Year", ISNamespace::FT_Year, "INTEGER", "ISYearEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("PhoneBase", ISNamespace::FT_PhoneBase, "CHARACTER VARYING", "ISPhoneBaseEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Phone", ISNamespace::FT_Phone, "CHARACTER VARYING", "ISPhoneEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("IPAddress", ISNamespace::FT_IPAddress, "CHARACTER VARYING", "ISIPAddressEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Password", ISNamespace::FT_Password, "CHARACTER VARYING", "ISPasswordEdit", "ISComboSearchString", false);
	AssociationTypes.Insert("Image", ISNamespace::FT_Image, "BYTEA", "ISImageEdit", QString(), false);
	AssociationTypes.Insert("Image64", ISNamespace::FT_Image64, "CHARACTER VARYING", "ISImageEdit64", QString(), false);
	AssociationTypes.Insert("Color", ISNamespace::FT_Color, "CHARACTER VARYING", "ISColorEdit", QString(), false);
	AssociationTypes.Insert("EMail", ISNamespace::FT_EMail, "CHARACTER VARYING", "ISEMailEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Passport", ISNamespace::FT_Passport, "CHARACTER VARYING", "ISPassportEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Volume", ISNamespace::FT_Volume, "INTEGER", "ISVolumeEdit", QString(), false);
	AssociationTypes.Insert("Birthday", ISNamespace::FT_Birthday, "DATE", "ISBirthdayEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("Seconds", ISNamespace::FT_Seconds, "INTEGER", "ISIntegerEdit", "ISComboSearchNumber", false);
	AssociationTypes.Insert("CallDetails", ISNamespace::FT_CallDetails, "BOOLEAN", QString(), QString(), false);
	AssociationTypes.Insert("Month", ISNamespace::FT_Month, "INTEGER", "ISMonthEdit", "ISComboSearchNumber", false);
	AssociationTypes.Insert("Login", ISNamespace::FT_Login, "CHARACTER VARYING", "ISLoginEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("Percent", ISNamespace::FT_Percent, "NUMERIC", "ISPercentEdit", "ISComboSearchNumber", true);
	AssociationTypes.Insert("Address", ISNamespace::FT_Address, "CHARACTER VARYING", "ISAddressEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("Url", ISNamespace::FT_Url, "CHARACTER VARYING", "ISUrlEdit", "ISComboSearchString", true);
	AssociationTypes.Insert("File", ISNamespace::FT_File, "INTEGER", "ISFileEdit", QString(), false);
}
//-----------------------------------------------------------------------------
ISMetaData::~ISMetaData()
{
	while (!Resources.empty())
	{
		delete VectorTakeBack(Resources);
	}
}
//-----------------------------------------------------------------------------
ISMetaData& ISMetaData::GetInstanse()
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
				GenerateSqlFromForeigns();
			}
		}

		Initialized = Result;
	}
	return Result;
}
//-----------------------------------------------------------------------------
PMetaClassTable* ISMetaData::GetMetaTable(const QString &TableName)
{
	if (TablesMap.count(TableName))
	{
		return TablesMap[TableName];
	}
	IS_ASSERT(false, QString("Not found meta table \"" + TableName + "\""));
	return nullptr;
}
//-----------------------------------------------------------------------------
PMetaClassTable* ISMetaData::GetMetaQuery(const QString &QueryName)
{
	if (QueriesMap.count(QueryName))
	{
		return QueriesMap[QueryName];
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
PMetaClassField* ISMetaData::GetMetaField(PMetaClassTable *MetaTable, const QString &FieldName)
{
	for (PMetaClassField *MetaField : MetaTable->AllFields)
	{
		if (MetaField->Name.toLower() == FieldName.toLower())
		{
			return MetaField;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
PMetaClassField* ISMetaData::GetMetaField(const QString &TableName, const QString &FieldName)
{
	return GetMetaField(TablesMap[TableName], FieldName);
}
//-----------------------------------------------------------------------------
std::vector<PMetaClassFunction*> ISMetaData::GetFunctions()
{
	return ConvertMapToValues<QString, PMetaClassFunction *>(FunctionsMap);
}
//-----------------------------------------------------------------------------
std::vector<PMetaClassTable*> ISMetaData::GetTables()
{
	return ConvertMapToValues<QString, PMetaClassTable *>(TablesMap);
}
//-----------------------------------------------------------------------------
ISVectorString ISMetaData::GetMetaQueries()
{
	ISVectorString Vector(QueriesMap.size());
	size_t Index = 0;
	for (const auto &MapItem : QueriesMap)
	{
		Vector[Index] = ISMetaViewQuery(MapItem.first).GetQueryText();
		++Index;
	}
	return Vector;
}
//-----------------------------------------------------------------------------
std::vector<PMetaClassIndex*> ISMetaData::GetSystemIndexes()
{
	std::vector<PMetaClassIndex*> SystemIndexes;
	for (PMetaClassTable *MetaTable : GetTables()) //Обход таблиц
	{
		for (PMetaClassField *MetaField : MetaTable->SystemFields) //Обход полей
		{
			if (MetaField->Index)
			{
				SystemIndexes.emplace_back(MetaField->Index);
			}
		}
	}
	return SystemIndexes;
}
//-----------------------------------------------------------------------------
std::vector<PMetaClassIndex*> ISMetaData::GetIndexes()
{
	std::vector<PMetaClassIndex*> Indexes;
	for (PMetaClassTable *MetaTable : GetTables()) //Обход таблиц
	{
		for (PMetaClassField *MetaField : MetaTable->Fields) //Обход полей
		{
			if (MetaField->Index)
			{
				Indexes.emplace_back(MetaField->Index);
			}
		}
	}
	return Indexes;
}
//-----------------------------------------------------------------------------
std::vector<PMetaClassIndex*> ISMetaData::GetCompoundIndexes()
{
	return IndexesCompound;
}
//-----------------------------------------------------------------------------
std::vector<PMetaClassForeign*> ISMetaData::GetForeigns()
{
	std::vector<PMetaClassForeign*> Foreigns;
	for (PMetaClassTable *MetaTable : GetTables()) //Обход таблиц
	{
		for (PMetaClassField *MetaField : MetaTable->Fields) //Обход полей
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
std::vector<PMetaClassResource*> ISMetaData::GetResources()
{
	return Resources;
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckExistTable(const QString &TableName) const
{
	return TablesMap.find(TableName) != TablesMap.end();
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckExitField(PMetaClassTable *MetaTable, const QString &FieldName) const
{
	for (PMetaClassField *MetaField : MetaTable->AllFields)
	{
		if (MetaField->Name.toLower() == FieldName.toLower())
		{
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
ISAssociationTypes& ISMetaData::GetAssociationTypes()
{
	return AssociationTypes;
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
		PMetaClassTable *MetaTable = MapItem.second;
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
void ISMetaData::GenerateSqlFromForeigns()
{
	std::vector<PMetaClassForeign*> Foreigns = GetForeigns();
	for (PMetaClassForeign *MetaForeign : Foreigns)
	{
		MetaForeign->SqlQuery = ISMetaDataHelper::GenerateSqlQueryFromForeign(MetaForeign); //Генерация SQL-запроса для внешнего ключа
	}
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
		PMetaClassTable *MetaTable = new PMetaClassTable();
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
				MetaTable->UseRoles = QVariant(DomNamedNodeMap.namedItem("Name").nodeValue()).toBool();
				MetaTable->ClassFilter = DomNamedNodeMap.namedItem("ClassFilter").nodeValue();
				MetaTable->ClassFilterField = DomNamedNodeMap.namedItem("ClassFilterField").nodeValue();
				MetaTable->ObjectForm = DomNamedNodeMap.namedItem("ObjectForm").nodeValue();
				MetaTable->ShowOnly = QVariant(DomNamedNodeMap.namedItem("ShowOnly").nodeValue()).toBool();
				MetaTable->IsSystem = QVariant(DomNamedNodeMap.namedItem("IsSystem").nodeValue()).toBool();
				MetaTable->SqlModel = DomNamedNodeMap.namedItem("SqlModel").nodeValue();
				MetaTable->Parent = DomNamedNodeMap.namedItem("Parent").nodeValue();
				MetaTable->Where = DomNamedNodeMap.namedItem("Where").nodeValue();
				MetaTable->OrderField = DomNamedNodeMap.namedItem("OrderField").nodeValue();

				if (Parent.isEmpty()) //Если мета-таблица не является запросом, проинициализировать системные поля
				{
					InitializeXSNTableSystemFields(MetaTable); //Инициализация системных полей
				}

				Result = InitializeXSNTableSystemFieldsVisible(MetaTable, GetChildDomNode(DomNode, "IncludeSystemFields").firstChild()); //Инициализация видимости системных полей
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
					Result = InitializeXSNTableJoins(MetaTable, GetChildDomNode(DomNode, "Joins").firstChild()); //Инициализация JOIN'ов
				}

				if (Parent.length()) //Если у мета-таблицы есть родительская таблица - мета-таблица является запросом
				{
					Result = !QueriesMap.count(TableName);
					if (Result)
					{
						QueriesMap.emplace(TableName, MetaTable);
					}
					else
					{
						ErrorString = QString("Query \"%1\" already exist in meta data").arg(TableName);
					}
				}
				else //У мета-таблицы нет родительской таблицы
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
										Result = !MetaTable->TitleName.isEmpty();
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
										else
										{
											ErrorString = QString("Empty table \"%1\" title name.").arg(MetaTable->Name);
										}
									}
									else
									{
										ErrorString = QString("Empty table \"%1\" local list name.").arg(MetaTable->Name);
									}
								}
								else
								{
									ErrorString = QString("Empty table \"%1\" local name.").arg(MetaTable->Name);
								}
							}
							else
							{
								ErrorString = QString("Empty table \"%1\" alias.").arg(MetaTable->Name);
							}
						}
						else
						{
							ErrorString = QString("Empty table \"%1\" uid.").arg(MetaTable->Name);
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
void ISMetaData::InitializeXSNTableSystemFields(PMetaClassTable *MetaTable)
{
    QFile File(PATH_SCHEMA_TEMPLATE_FIELDS);
    IS_ASSERT(File.open(QIODevice::ReadOnly), File.errorString());
    QString Content = File.readAll();
    File.close();

    QDomElement DomElement = ISSystem::GetDomElement(Content);
	QDomNode DomNode = DomElement.firstChild();
	InitializeXSNTableFields(MetaTable, DomNode);

	PMetaClassField *FieldID = MetaTable->GetField("ID");
	IS_ASSERT(FieldID, "Null field object");
	FieldID->Index = new PMetaClassIndex(true, MetaTable->Alias, MetaTable->Name, FieldID->Name);

	PMetaClassField *FieldUID = MetaTable->GetField("UID");
	IS_ASSERT(FieldUID, "Null field object");
	FieldUID->Index = new PMetaClassIndex(true, MetaTable->Alias, MetaTable->Name, FieldUID->Name);
	
	PMetaClassField *FieldIsDeleted = MetaTable->GetField("IsDeleted");
	IS_ASSERT(FieldIsDeleted, "Null field object");
	FieldIsDeleted->Index = new PMetaClassIndex(false, MetaTable->Alias, MetaTable->Name, FieldIsDeleted->Name);

	PMetaClassField *FieldIsSystem = MetaTable->GetField("IsSystem");
	IS_ASSERT(FieldIsSystem, "Null field object");
	FieldIsSystem->Index = new PMetaClassIndex(false, MetaTable->Alias, MetaTable->Name, FieldIsSystem->Name);

	PMetaClassField *FieldCreationDate = MetaTable->GetField("CreationDate");
	IS_ASSERT(FieldCreationDate, "Null field object");
	FieldCreationDate->Index = new PMetaClassIndex(false, MetaTable->Alias, MetaTable->Name, FieldCreationDate->Name);

	PMetaClassField *FieldUpdationDate = MetaTable->GetField("UpdationDate");
	IS_ASSERT(FieldUpdationDate, "Null field object");
	FieldUpdationDate->Index = new PMetaClassIndex(false, MetaTable->Alias, MetaTable->Name, FieldUpdationDate->Name);

	PMetaClassField *FieldDeletionDate = MetaTable->GetField("DeletionDate");
	IS_ASSERT(FieldDeletionDate, "Null field object");
	FieldDeletionDate->Index = new PMetaClassIndex(false, MetaTable->Alias, MetaTable->Name, FieldDeletionDate->Name);

	PMetaClassField *FieldCreationUser = MetaTable->GetField("CreationUser");
	IS_ASSERT(FieldCreationUser, "Null field object");
	FieldCreationUser->Index = new PMetaClassIndex(false, MetaTable->Alias, MetaTable->Name, FieldCreationUser->Name);

	PMetaClassField *FieldUpdationUser = MetaTable->GetField("UpdationUser");
	IS_ASSERT(FieldUpdationUser, "Null field object");
	FieldUpdationUser->Index = new PMetaClassIndex(false, MetaTable->Alias, MetaTable->Name, FieldUpdationUser->Name);

	PMetaClassField *FieldDeletionUser = MetaTable->GetField("DeletionUser");
	IS_ASSERT(FieldDeletionUser, "Null field object");
	FieldDeletionUser->Index = new PMetaClassIndex(false, MetaTable->Alias, MetaTable->Name, FieldDeletionUser->Name);
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableSystemFieldsVisible(PMetaClassTable *MetaTable, const QDomNode &DomNode)
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

		PMetaClassField *MetaField = MetaTable->GetField(FieldName);
		Result = MetaField ? true : false;
		if (!Result)
		{
			ErrorString = QString("Not found visible system field %1. File: %2. Line: %3").arg(FieldName).arg(CurrentXSN).arg(Temp.lineNumber());
			break;
		}

		QString LocalName = Temp.attributes().namedItem("LocalName").nodeValue();
		if (LocalName.length())
		{
			MetaField->LabelName = LocalName;
			MetaField->LocalListName = LocalName;
		}

		MetaTable->SystemFieldsVisible.emplace_back(MetaField);
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableFields(PMetaClassTable *MetaTable, const QDomNode &DomNode)
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
			
			Result = !FieldName.contains(SYMBOL_SPACE);
			if (!Result)
			{
				ErrorString = QString("Forbidden symbol ' ' in field name: %1").arg(FieldName);
				break;
			}
			
			QDomNamedNodeMap DomNamedNodeMap = Temp.attributes();
			PMetaClassField *MetaField = new PMetaClassField();
			MetaField->UID = DomNamedNodeMap.namedItem("UID").nodeValue();
			MetaField->Name = FieldName;
			MetaField->Type = ISMetaData::GetInstanse().GetAssociationTypes().GetTypeField(DomNamedNodeMap.namedItem("Type").nodeValue());
			MetaField->Size = DomNamedNodeMap.namedItem("LocalName").nodeValue().toInt();
			MetaField->Upper = QVariant(DomNamedNodeMap.namedItem("LocalListName").nodeValue()).toBool();
			MetaField->Lower = QVariant(DomNamedNodeMap.namedItem("TitleName").nodeValue()).toBool();
			MetaField->DefaultValue = DomNamedNodeMap.namedItem("DefaultValue").nodeValue().isEmpty() ? QVariant() : DomNamedNodeMap.namedItem("DefaultValue").nodeValue();
			MetaField->DefaultValueWidget = DomNamedNodeMap.namedItem("DefaultValueWidget").nodeValue().isEmpty() ? QVariant() : DomNamedNodeMap.namedItem("DefaultValueWidget").nodeValue();
			MetaField->LabelName = DomNamedNodeMap.namedItem("LabelName").nodeValue();
			MetaField->LocalListName = DomNamedNodeMap.namedItem("LocalListName").nodeValue();
			MetaField->NotNull = QVariant(DomNamedNodeMap.namedItem("NotNull").nodeValue()).toBool();
			MetaField->ReadOnly = QVariant(DomNamedNodeMap.namedItem("ReadOnly").nodeValue()).toBool();
			MetaField->HideFromObject = QVariant(DomNamedNodeMap.namedItem("HideFromObject").nodeValue()).toBool();
			MetaField->HideFromList = QVariant(DomNamedNodeMap.namedItem("HideFromList").nodeValue()).toBool();
			MetaField->NotSearch = QVariant(DomNamedNodeMap.namedItem("NotSearch").nodeValue()).toBool();
			MetaField->Hint = DomNamedNodeMap.namedItem("OrderField").nodeValue();
			MetaField->PlaceholderText = DomNamedNodeMap.namedItem("PlaceholderText").nodeValue();
			MetaField->ControlWidget = DomNamedNodeMap.namedItem("ControlWidget").nodeValue();
			MetaField->RegExp = DomNamedNodeMap.namedItem("RegExp").nodeValue();
			MetaField->IsSystem = QVariant(DomNamedNodeMap.namedItem("IsSystem").nodeValue()).toBool();
			MetaField->QueryText = DomNamedNodeMap.namedItem("QueryText").nodeValue();
			MetaField->Sequence = QVariant(DomNamedNodeMap.namedItem("Sequence").nodeValue()).toBool();
			MetaField->LayoutName = DomNamedNodeMap.namedItem("LayoutName").nodeValue();
			MetaField->SeparatorName = DomNamedNodeMap.namedItem("SeparatorName").nodeValue();
			MetaField->IsSystem ? MetaTable->SystemFields.emplace_back(MetaField) : MetaTable->Fields.emplace_back(MetaField);

			if (MetaTable->Parent.isEmpty() && MetaField->QueryText.isEmpty())
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

			Result = !MetaTable->GetField(FieldName);
			if (!Result)
			{
				ErrorString = QString("Field \"%1\" already exist. TableName: %2").arg(FieldName).arg(MetaTable->Name);
				break;
			}
			MetaTable->AllFields.emplace_back(MetaField);
		}
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableIndexes(PMetaClassTable *MetaTable, const QDomNode &DomNode)
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
				PMetaClassIndex *Index = new PMetaClassIndex(Unique, MetaTable->Alias, MetaTable->Name, QString());
				for (const QString &IndexName : IndexList)
				{
					Index->Fields.emplace_back(IndexName);
				}
				IndexesCompound.emplace_back(Index);
			}
			else //Индекс стандартный
			{
				PMetaClassField *MetaField = MetaTable->GetField(FieldName);
				Result = MetaField ? true : false;
				if (!Result)
				{
					QString("Not found field from name: %1").arg(FieldName);
					break;
				}

				Result = !MetaField->Index;
				if (!Result)
				{
					ErrorString = QString("Index already exist. TableName: %1. FieldName: %2.").arg(MetaTable->Name).arg(FieldName);
					break;
				}
				MetaField->Index = new PMetaClassIndex(Unique, MetaTable->Alias, MetaTable->Name, FieldName);
			}
		}
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableForeigns(PMetaClassTable *MetaTable, const QDomNode &DomNode)
{
	bool Result = true;
	QDomNode Temp = DomNode;
	while (!Temp.isNull())
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

		Result = MetaTable->GetField(FieldName) ? true : false;
		if (!Result)
		{
			ErrorString = QString("Not found field \"%1\" in table \"%2\"").arg(FieldName).arg(MetaTable->Name);
			break;
		}

		QDomNamedNodeMap DomNamedNodeMap = Temp.attributes();
		PMetaClassForeign *MetaForeign = new PMetaClassForeign();
		MetaForeign->Field = DomNamedNodeMap.namedItem("Field").nodeValue();
		MetaForeign->ForeignClass = DomNamedNodeMap.namedItem("ForeignClass").nodeValue();
		MetaForeign->ForeignField = DomNamedNodeMap.namedItem("ForeignField").nodeValue();
		MetaForeign->ForeignViewNameField = DomNamedNodeMap.namedItem("ForeignViewNameField").nodeValue();
		MetaForeign->OrderField = DomNamedNodeMap.namedItem("OrderField").nodeValue();
		MetaForeign->TableName = MetaTable->Name;

		PMetaClassField *MetaField = MetaTable->GetField(FieldName);
		Result = MetaTable ? true : false;
		if (!Result)
		{
			ErrorString = QString("Not found field \"%1\" in table \"%2\"").arg(FieldName).arg(MetaTable->Name);
			break;
		}
		
		Result = !MetaField->Foreign;
		if (!Result)
		{
			ErrorString = QString("Foreign already exist. TableName: %1. FieldName: %2.").arg(MetaTable->Name).arg(FieldName);
			break;
		}

		MetaField->Foreign = MetaForeign;
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableEscorts(PMetaClassTable *MetaTable, const QDomNode &DomNode)
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
		PMetaClassEscort *MetaEscort = new PMetaClassEscort();
		MetaEscort->LocalName = DomNamedNodeMap.namedItem("LocalName").nodeValue();
		MetaEscort->TableName = DomNamedNodeMap.namedItem("TableName").nodeValue();
		MetaEscort->ClassName = DomNamedNodeMap.namedItem("ClassName").nodeValue();
		MetaEscort->ClassFilter = DomNamedNodeMap.namedItem("ClassFilter").nodeValue();
		MetaTable->Escorts.emplace_back(MetaEscort);
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSNTableJoins(PMetaClassTable *MetaTable, const QDomNode &DomNode)
{
	bool Result = true;
	QDomNode Temp = DomNode;
	while (!Temp.isNull())
	{
		Result = !Temp.attributes().isEmpty();
		if (!Result)
		{
			ErrorString = QString("Empty attributes field. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
			break;
		}

		QString JoinText = Temp.attributes().namedItem("Text").nodeValue();
		Result = !JoinText.isEmpty();
		if (!Result)
		{
			ErrorString = QString("Empty join text. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber());
			break;
		}
		MetaTable->Joins.emplace_back(JoinText);
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

						PMetaClassResource *MetaResource = new PMetaClassResource();
						MetaResource->TableName = TableName;
						MetaResource->UID = DomNamedNodeMap.namedItem("UID").nodeValue();
						DomNamedNodeMap.removeNamedItem("UID");

						for (int i = 0, c = DomNamedNodeMap.size(); i < c; ++i) //Обход полей ресурса
						{
							QDomNode DomItem = DomNamedNodeMap.item(i); // Поле-значение
							QString FieldName = DomItem.nodeName(); //Имя поля
							QString Value = DomItem.nodeValue(); //Значение поля

							PMetaClassTable *MetaTable = TablesMap[TableName];
							Result = MetaTable ? true : false;
							if (!Result)
							{
								ErrorString = QString("Invalid table name \"%1\" in resource. File: %2. Line: %3").arg(TableName).arg(CurrentXSR).arg(DomNode.lineNumber());
								break;
							}

							PMetaClassField *MetaField = MetaTable->GetField(FieldName);
							Result = MetaField ? true : false;
							if (Result)
							{
								MetaResource->AddField(FieldName, Value);
							}
							else
							{
								ErrorString = QString("Invalid field name \"%1\" in resource. File: %2. Line: %3").arg(FieldName).arg(CurrentXSR).arg(DomNode.lineNumber());
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
						PMetaClassFunction *MetaFunction = new PMetaClassFunction();
						MetaFunction->Text = DomNode.attributes().namedItem("Text").nodeValue();
						FunctionsMap.emplace(DomNode.attributes().namedItem("Name").nodeValue(), MetaFunction);
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
