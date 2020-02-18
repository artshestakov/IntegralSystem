#include "ISMetaData.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
#include "ISMetaDataHelper.h"
#include "ISUuid.h"
#include "ISMetaUuidCheckeder.h"
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
	while (!Resources.isEmpty())
	{
		delete Resources.takeLast();
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
bool ISMetaData::Initialize(const QString &configuration, bool InitXSR, bool InitXSF)
{
	bool Result = Initialized;
	if (!Result)
	{
		Configuration = configuration;

		ISDebug::ShowDebugString("Initialize MetaData...");

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

		Result = CheckUniqueAllIdentifiers(InitXSR);
		if (Result)
		{
			Result = CheckUniqueAllAliases();
		}
		
		if (Result)
		{
			GenerateSqlFromForeigns();
		}

		Initialized = Result;
	}
	return Result;
}
//-----------------------------------------------------------------------------
PMetaClassTable* ISMetaData::GetMetaTable(const QString &TableName)
{
	PMetaClassTable *MetaTable = TablesMap.value(TableName);
	IS_ASSERT(MetaTable, QString("Not found meta table: %1").arg(TableName));
	return MetaTable;
}
//-----------------------------------------------------------------------------
PMetaClassTable* ISMetaData::GetMetaQuery(const QString &QueryName)
{
	PMetaClassTable *MetaQuery = QueriesMap.value(QueryName);
	IS_ASSERT(MetaQuery, QString("Not found query: %1").arg(QueryName));
	return MetaQuery;
}
//-----------------------------------------------------------------------------
PMetaClassField* ISMetaData::GetMetaField(PMetaClassTable *MetaTable, const QString &FieldName)
{
	PMetaClassField *Result = nullptr;
	for (PMetaClassField *MetaField : MetaTable->GetAllFields())
	{
		if (MetaField->GetName().toLower() == FieldName.toLower())
		{
			Result = MetaField;
			break;
		}
	}
	IS_ASSERT(Result, QString("Field \"%1\" not found in meta table \"%2\"").arg(FieldName).arg(MetaTable->GetName()));
	return Result;
}
//-----------------------------------------------------------------------------
PMetaClassField* ISMetaData::GetMetaField(const QString &TableName, const QString &FieldName)
{
	PMetaClassField *MetaField = GetMetaField(TablesMap.value(TableName), FieldName);
	IS_ASSERT(MetaField, QString("Field \"%1\" not found in meta table \"%2\"").arg(FieldName).arg(TableName));
	return MetaField;
}
//-----------------------------------------------------------------------------
QList<PMetaClassFunction*> ISMetaData::GetFunctions()
{
	return FunctionsMap.values();
}
//-----------------------------------------------------------------------------
QList<PMetaClassTable*> ISMetaData::GetTables()
{
	return TablesMap.values();
}
//-----------------------------------------------------------------------------
QVectorString ISMetaData::GetMetaQueries()
{
	QVectorString VectorString;
	for (const auto &MapItem : QueriesMap.toStdMap())
	{
		PMetaClassTable *MetaClassQuery = MapItem.second;
		VectorString.append(MetaClassQuery->GetName());
	}
	return VectorString;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassIndex*> ISMetaData::GetSystemIndexes()
{
	QVector<PMetaClassIndex*> SystemIndexes;
	for (PMetaClassTable *MetaTable : GetTables()) //Обход таблиц
	{
		for (PMetaClassField *MetaField : MetaTable->GetSystemFields()) //Обход полей
		{
			PMetaClassIndex *SystemIndex = MetaField->GetIndex();
			if (SystemIndex)
			{
				SystemIndexes.append(SystemIndex);
			}
		}
	}
	return SystemIndexes;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassIndex*> ISMetaData::GetIndexes()
{
	QVector<PMetaClassIndex*> Indexes;
	for (PMetaClassTable *MetaTable : GetTables()) //Обход таблиц
	{
		for (PMetaClassField *MetaField : MetaTable->GetFields()) //Обход полей
		{
			PMetaClassIndex *Index = MetaField->GetIndex();
			if (Index)
			{
				Indexes.append(Index);
			}
		}
	}
	return Indexes;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassIndex*> ISMetaData::GetCompoundIndexes()
{
	return IndexesCompound;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassForeign*> ISMetaData::GetForeigns()
{
	QVector<PMetaClassForeign*> Foreigns;
	for (PMetaClassTable *MetaTable : GetTables()) //Обход таблиц
	{
		for (PMetaClassField *MetaField : MetaTable->GetFields()) //Обход полей
		{
			PMetaClassForeign *Foreign = MetaField->GetForeign();
			if (Foreign)
			{
				Foreigns.append(Foreign);
			}
		}
	}
	return Foreigns;
}
//-----------------------------------------------------------------------------
QVector<PMetaClassResource*> ISMetaData::GetResources()
{
	return Resources;
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckExistTable(const QString &TableName) const
{
	return TablesMap.value(TableName) ? true : false;
}
//-----------------------------------------------------------------------------
bool ISMetaData::CheckExitField(PMetaClassTable *MetaTable, const QString &FieldName) const
{
	bool Result = false;
	for (PMetaClassField *MetaField : MetaTable->GetAllFields())
	{
		if (MetaField->GetName().toLower() == FieldName.toLower())
		{
			Result = true;
			break;
		}
	}
	return Result;
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

	if (Configuration.length())
	{
		FileInfoList.append(QDir(":_" + Configuration).entryInfoList(FilterXsnXsr, QDir::Files));
	}

	ISMetaUuidCheckeder MetaUuidCheckeder;
	bool Result = true;
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
	for (const auto &MapItem : TablesMap.toStdMap()) //Обход таблиц
	{
		PMetaClassTable *MetaTable = MapItem.second;
		Result = MetaTable ? true : false;
		if (!Result)
		{
			ErrorString = "Error";
			break;
		}

		QString Alias = MetaTable->GetAlias().toLower();
		Result = !StringList.contains(Alias);
		if (Result)
		{
			StringList.append(Alias);
		}
		else
		{
			ErrorString = QString("Duplicate alias: %1. Table: %2.").arg(Alias).arg(MetaTable->GetName());
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISMetaData::GenerateSqlFromForeigns()
{
	QVector<PMetaClassForeign*> Foreigns = GetForeigns();
	for (PMetaClassForeign *MetaForeign : Foreigns)
	{
		MetaForeign->SetSqlQuery(ISMetaDataHelper::GenerateSqlQueryFromForeign(MetaForeign)); //Генерация SQL-запроса для внешнего ключа
	}
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSN()
{
	QStringList Filter("*.xsn");
	QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //Загрузка мета-данных движка

	bool Result = !Configuration.isEmpty();
	if (Result)
	{
		FileInfoList.append(QDir(":_" + Configuration).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка мета-данных конфигурации
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
				MetaTable->SetName(DomNamedNodeMap.namedItem("Name").nodeValue());
				MetaTable->SetUID(DomNamedNodeMap.namedItem("UID").nodeValue());
				MetaTable->SetAlias(DomNamedNodeMap.namedItem("Alias").nodeValue());
				MetaTable->SetLocalName(DomNamedNodeMap.namedItem("LocalName").nodeValue());
				MetaTable->SetLocalListName(DomNamedNodeMap.namedItem("LocalListName").nodeValue());
				MetaTable->SetTitleName(DomNamedNodeMap.namedItem("TitleName").nodeValue());
				MetaTable->SetUseRoles(QVariant(DomNamedNodeMap.namedItem("Name").nodeValue()).toBool());
				MetaTable->SetClassFilter(DomNamedNodeMap.namedItem("ClassFilter").nodeValue());
				MetaTable->SetClassFilterField(DomNamedNodeMap.namedItem("ClassFilterField").nodeValue());
				MetaTable->SetObjectForm(DomNamedNodeMap.namedItem("ObjectForm").nodeValue());
				MetaTable->SetShowOnly(QVariant(DomNamedNodeMap.namedItem("ShowOnly").nodeValue()).toBool());
				MetaTable->SetIsSystem(QVariant(DomNamedNodeMap.namedItem("IsSystem").nodeValue()).toBool());
				MetaTable->SetSqlModel(DomNamedNodeMap.namedItem("SqlModel").nodeValue());
				MetaTable->SetParent(DomNamedNodeMap.namedItem("Parent").nodeValue());
				MetaTable->SetWhere(DomNamedNodeMap.namedItem("Where").nodeValue());
				MetaTable->SetOrderField(DomNamedNodeMap.namedItem("OrderField").nodeValue());

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
					Result = !QueriesMap.contains(TableName);
					if (Result)
					{
						QueriesMap.insert(TableName, MetaTable);
					}
					else
					{
						ErrorString = QString("Query \"%1\" already exist in meta data").arg(TableName);
					}
				}
				else //У мета-таблицы нет родительской таблицы
				{
					//Проверка на заполнение обязательных атрибутов
					Result = !MetaTable->GetName().isEmpty();
					if (Result)
					{
						Result = !MetaTable->GetUID().isEmpty();
						if (Result)
						{
							Result = !MetaTable->GetAlias().isEmpty();
							if (Result)
							{
								Result = !MetaTable->GetLocalName().isEmpty();
								if (Result)
								{
									Result = !MetaTable->GetLocalListName().isEmpty();
									if (Result)
									{
										Result = !MetaTable->GetTitleName().isEmpty();
										if (Result)
										{
											Result = !TablesMap.contains(TableName);
											if (Result)
											{
												TablesMap.insert(TableName, MetaTable);
											}
											else
											{
												ErrorString = QString("Table \"%1\" already exist in meta data").arg(TableName);
											}
										}
										else
										{
											ErrorString = QString("Empty table \"%1\" title name.").arg(MetaTable->GetName());
										}
									}
									else
									{
										ErrorString = QString("Empty table \"%1\" local list name.").arg(MetaTable->GetName());
									}
								}
								else
								{
									ErrorString = QString("Empty table \"%1\" local name.").arg(MetaTable->GetName());
								}
							}
							else
							{
								ErrorString = QString("Empty table \"%1\" alias.").arg(MetaTable->GetName());
							}
						}
						else
						{
							ErrorString = QString("Empty table \"%1\" uid.").arg(MetaTable->GetName());
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
	FieldID->SetIndex(new PMetaClassIndex(true, MetaTable->GetAlias(), MetaTable->GetName(), FieldID->GetName()));

	PMetaClassField *FieldUID = MetaTable->GetField("UID");
	IS_ASSERT(FieldUID, "Null field object");
	FieldUID->SetIndex(new PMetaClassIndex(true, MetaTable->GetAlias(), MetaTable->GetName(), FieldUID->GetName()));
	
	PMetaClassField *FieldIsDeleted = MetaTable->GetField("IsDeleted");
	IS_ASSERT(FieldIsDeleted, "Null field object");
	FieldIsDeleted->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldIsDeleted->GetName()));

	PMetaClassField *FieldIsSystem = MetaTable->GetField("IsSystem");
	IS_ASSERT(FieldIsSystem, "Null field object");
	FieldIsSystem->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldIsSystem->GetName()));

	PMetaClassField *FieldCreationDate = MetaTable->GetField("CreationDate");
	IS_ASSERT(FieldCreationDate, "Null field object");
	FieldCreationDate->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldCreationDate->GetName()));

	PMetaClassField *FieldUpdationDate = MetaTable->GetField("UpdationDate");
	IS_ASSERT(FieldUpdationDate, "Null field object");
	FieldUpdationDate->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldUpdationDate->GetName()));

	PMetaClassField *FieldDeletionDate = MetaTable->GetField("DeletionDate");
	IS_ASSERT(FieldDeletionDate, "Null field object");
	FieldDeletionDate->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldDeletionDate->GetName()));

	PMetaClassField *FieldCreationUser = MetaTable->GetField("CreationUser");
	IS_ASSERT(FieldCreationUser, "Null field object");
	FieldCreationUser->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldCreationUser->GetName()));

	PMetaClassField *FieldUpdationUser = MetaTable->GetField("UpdationUser");
	IS_ASSERT(FieldUpdationUser, "Null field object");
	FieldUpdationUser->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldUpdationUser->GetName()));

	PMetaClassField *FieldDeletionUser = MetaTable->GetField("DeletionUser");
	IS_ASSERT(FieldDeletionUser, "Null field object");
	FieldDeletionUser->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldDeletionUser->GetName()));
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
			MetaField->SetLabelName(LocalName);
			MetaField->SetLocalListName(LocalName);
		}

		MetaTable->AddVisibleSystemField(MetaField);
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
			MetaField->SetUID(DomNamedNodeMap.namedItem("UID").nodeValue());
			MetaField->SetName(FieldName);
			MetaField->SetType(DomNamedNodeMap.namedItem("Type").nodeValue());
			MetaField->SetSize(DomNamedNodeMap.namedItem("LocalName").nodeValue().toInt());
			MetaField->SetUpper(QVariant(DomNamedNodeMap.namedItem("LocalListName").nodeValue()).toBool());
			MetaField->SetLower(QVariant(DomNamedNodeMap.namedItem("TitleName").nodeValue()).toBool());
			MetaField->SetDefaultValue(DomNamedNodeMap.namedItem("DefaultValue").nodeValue());
			MetaField->SetDefaultValueWidget(DomNamedNodeMap.namedItem("DefaultValueWidget").nodeValue());
			MetaField->SetLabelName(DomNamedNodeMap.namedItem("LabelName").nodeValue());
			MetaField->SetLocalListName(DomNamedNodeMap.namedItem("LocalListName").nodeValue());
			MetaField->SetNotNull(QVariant(DomNamedNodeMap.namedItem("NotNull").nodeValue()).toBool());
			MetaField->SetReadOnly(QVariant(DomNamedNodeMap.namedItem("ReadOnly").nodeValue()).toBool());
			MetaField->SetHideFromObject(QVariant(DomNamedNodeMap.namedItem("HideFromObject").nodeValue()).toBool());
			MetaField->SetHideFromList(QVariant(DomNamedNodeMap.namedItem("HideFromList").nodeValue()).toBool());
			MetaField->SetNotSearch(QVariant(DomNamedNodeMap.namedItem("NotSearch").nodeValue()).toBool());
			MetaField->SetHint(DomNamedNodeMap.namedItem("OrderField").nodeValue());
			MetaField->SetPlaceholderText(DomNamedNodeMap.namedItem("PlaceholderText").nodeValue());
			MetaField->SetControlWidget(DomNamedNodeMap.namedItem("ControlWidget").nodeValue());
			MetaField->SetRegExp(DomNamedNodeMap.namedItem("RegExp").nodeValue());
			MetaField->SetIsSystem(QVariant(DomNamedNodeMap.namedItem("IsSystem").nodeValue()).toBool());
			MetaField->SetQueryText(DomNamedNodeMap.namedItem("QueryText").nodeValue());
			MetaField->SetSequence(QVariant(DomNamedNodeMap.namedItem("Sequence").nodeValue()).toBool());
			MetaField->SetLayoutName(DomNamedNodeMap.namedItem("LayoutName").nodeValue());
			MetaField->SetSeparatorName(DomNamedNodeMap.namedItem("SeparatorName").nodeValue());
			MetaField->GetIsSystem() ? MetaTable->AddSystemField(MetaField) : MetaTable->AddField(MetaField);

			if (!MetaTable->GetParent().length() && !MetaField->GetQueryText().length())
			{
				//Проверка на заполнение обязательных атрибутов
				Result = !MetaField->GetUID().isEmpty();
				if (!Result)
				{
					ErrorString = QString("Empty field uid.");
					break;
				}

				Result = !MetaField->GetName().isEmpty();
				if (!Result)
				{
					ErrorString = QString("Empty field name.");
					break;
				}
				
				Result = !MetaField->GetLabelName().isEmpty();
				if (!Result)
				{
					ErrorString = QString("Empty field \"%1\" label name.").arg(MetaField->GetUID());
					break;
				}
				
				Result = !MetaField->GetLocalListName().isEmpty();
				if (!Result)
				{
					ErrorString = QString("Empty field \"%1\" local list name.").arg(MetaField->GetUID());
					break;
				}
			}

			Result = !MetaTable->GetField(FieldName);
			if (!Result)
			{
				ErrorString = QString("Field \"%1\" already exist. TableName: %2").arg(FieldName).arg(MetaTable->GetName());
				break;
			}
			MetaTable->AddAllField(MetaField);
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
				PMetaClassIndex *Index = new PMetaClassIndex(Unique, MetaTable->GetAlias(), MetaTable->GetName(), QString());
				for (const QString &IndexName : IndexList)
				{
					Index->AddField(IndexName);
				}
				IndexesCompound.append(Index);
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

				Result = !MetaField->GetIndex();
				if (!Result)
				{
					ErrorString = QString("Index already exist. TableName: %1. FieldName: %2.").arg(MetaTable->GetName()).arg(FieldName);
					break;
				}
				MetaField->SetIndex(new PMetaClassIndex(Unique, MetaTable->GetAlias(), MetaTable->GetName(), FieldName));
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
			ErrorString = QString("Not found field \"%1\" in table \"%2\"").arg(FieldName).arg(MetaTable->GetName());
			break;
		}

		QDomNamedNodeMap DomNamedNodeMap = Temp.attributes();
		PMetaClassForeign *MetaForeign = new PMetaClassForeign();
		MetaForeign->SetFieldName(DomNamedNodeMap.namedItem("Field").nodeValue());
		MetaForeign->SetForeignClass(DomNamedNodeMap.namedItem("ForeignClass").nodeValue());
		MetaForeign->SetForeignField(DomNamedNodeMap.namedItem("ForeignField").nodeValue());
		MetaForeign->SetForeignViewNameField(DomNamedNodeMap.namedItem("ForeignViewNameField").nodeValue());
		MetaForeign->SetOrderField(DomNamedNodeMap.namedItem("OrderField").nodeValue());
		MetaForeign->SetTableName(MetaTable->GetName());

		PMetaClassField *MetaField = MetaTable->GetField(FieldName);
		Result = MetaTable ? true : false;
		if (!Result)
		{
			ErrorString = QString("Not found field \"%1\" in table \"%2\"").arg(FieldName).arg(MetaTable->GetName());
			break;
		}
		
		Result = !MetaField->GetForeign();
		if (!Result)
		{
			ErrorString = QString("Foreign already exist. TableName: %1. FieldName: %2.").arg(MetaTable->GetName()).arg(FieldName);
			break;
		}

		MetaField->SetForeign(MetaForeign);
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
		MetaEscort->SetLocalName(DomNamedNodeMap.namedItem("LocalName").nodeValue());
		MetaEscort->SetTableName(DomNamedNodeMap.namedItem("TableName").nodeValue());
		MetaEscort->SetClassName(DomNamedNodeMap.namedItem("ClassName").nodeValue());
		MetaEscort->SetClassFilter(DomNamedNodeMap.namedItem("ClassFilter").nodeValue());
		MetaTable->AddEscort(MetaEscort);
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
		MetaTable->AddJoin(JoinText);
		Temp = Temp.nextSibling();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaData::InitializeXSR()
{
	QStringList Filter("*.xsr");
	QFileInfoList FileInfoList;
	FileInfoList.append(QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка ресурсов движка

	if (Configuration.length())
	{
		FileInfoList.append(QDir(":_" + Configuration).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка ресурсов конфигурации
	}

	bool Result = true;
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
						MetaResource->SetTableName(TableName);

						for (int i = 0, c = DomNamedNodeMap.size(); i < c; ++i) //Обход полей ресурса
						{
							QDomNode DomItem = DomNamedNodeMap.item(i); // Поле-значение
							QString FieldName = DomItem.nodeName(); //Имя поля
							QString Value = DomItem.nodeValue(); //Значение поля

							PMetaClassTable *MetaTable = TablesMap.value(TableName);
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
							Result = !MetaResource->GetUID().isEmpty();
							if (Result)
							{
								MetaResource->SetUID(MetaResource->GetUID());
								Resources.append(MetaResource);
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

	if (Configuration.length())
	{
		FileInfoList.append(QDir(":_" + Configuration).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка функций конфигурации
	}

	bool Result = true;
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
						MetaFunction->SetText(DomNode.attributes().namedItem("Text").nodeValue());
						FunctionsMap.insert(DomNode.attributes().namedItem("Name").nodeValue(), MetaFunction);
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
