#include "ISMetaData.h"
#include "EXDefines.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
#include "ISMetaDataHelper.h"
#include "ISUuid.h"
#include "ISMetaUuidCheckeder.h"
//-----------------------------------------------------------------------------
ISMetaData::ISMetaData() : QObject()
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

}
//-----------------------------------------------------------------------------
ISMetaData& ISMetaData::GetInstanse()
{
	static ISMetaData MetaData;
	return MetaData;
}
//-----------------------------------------------------------------------------
bool ISMetaData::GetInitialized() const
{
	return Initialized;
}
//-----------------------------------------------------------------------------
void ISMetaData::Initialize(const QString &configuration, bool InitXSR, bool InitXSF)
{
	IS_ASSERT(!Initialized, "Meta data already initialized.");
	Configuration = configuration;

	ISDebug::ShowDebugString("Initialize MetaData...");
	ISCountingTime CountingTime;

	InitializeXSN();

	if (InitXSR)
	{
		InitializeXSR();
	}

	if (InitXSF)
	{
		InitializeXSF();
	}

	CheckUniqueAllIdentifiers(InitXSR);
	CheckUniqueAllAliases();
	GenerateSqlFromForeigns();

	ISDebug::ShowDebugString(QString("Initialized MetaData %1 msec").arg(CountingTime.GetElapsed()));
	Initialized = true;
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
void ISMetaData::CheckUniqueAllIdentifiers(bool InitXSR)
{
	ISMetaUuidCheckeder MetaUuidCheckeder;

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

	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех файлов мета-данных
	{
        MetaUuidCheckeder.Search(FileInfo);
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::CheckUniqueAllAliases()
{
	QStringList StringList;
	for (const auto &MapItem : TablesMap.toStdMap()) //Обход таблиц
	{
		PMetaClassTable *MetaTable = MapItem.second;
		IS_ASSERT(MetaTable, "Error");

		QString Alias = MetaTable->GetAlias().toLower();
		IS_ASSERT(!StringList.contains(Alias), QString("Duplicate alias: %1. Table: %2.").arg(Alias).arg(MetaTable->GetName()));
		StringList.append(Alias);
	}
	StringList.clear();
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
void ISMetaData::InitializeXSN()
{
	QStringList Filter("*.xsn");
	QFileInfoList FileInfoList;
	FileInfoList.append(QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка мета-данных движка

	if (Configuration.length())
	{
		FileInfoList.append(QDir(":_" + Configuration).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка мета-данных конфигурации
	}

	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех XSN файлов
	{
		QFile FileXSN(FileInfo.filePath());
		IS_ASSERT(FileXSN.open(QIODevice::ReadOnly), QString("Not opened xsn file: %1").arg(FileInfo.filePath()));

		QString Content = FileXSN.readAll();
		FileXSN.close();

		InitializeXSN(Content);
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSN(const QString &Content)
{
	ISCountingTime Time;
	QDomElement DomElement = ISSystem::GetDomElement(Content);

	QString TagName = DomElement.tagName();
	IS_ASSERT(TagName.length(), "Empty tag name");
	IS_ASSERT(TagName == "XSN", QString("Invalid tag name \"%1\" in XSN file.").arg(TagName));

	QString SchemeName = DomElement.attributes().namedItem("Name").nodeValue();
	IS_ASSERT(SchemeName.length(), "Empty schema name.");

	CurrentXSN = SchemeName;

	QDomNode DomNode = DomElement.firstChild();
	while (!DomNode.isNull())
	{
		QDomNode CurrentNode = DomNode;

		if (CurrentNode.nodeName() == "Table")
		{
			InitializeXSNTable(CurrentNode);
		}
		else
		{
			IS_ASSERT(false, QString("Invalid NodeName \"%1\" in XSN file %1").arg(CurrentNode.nodeName()).arg(SchemeName));
		}

		DomNode = DomNode.nextSibling();
	}

	CurrentXSN.clear();
	ISDebug::ShowDebugString(QString("Init XSN %1: %2 msec").arg(SchemeName).arg(Time.GetElapsed()));
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSNTable(QDomNode &DomNode)
{
	IS_ASSERT(DomNode.attributes().length(), QString("Empty attributes table. File: %1. Line: %2").arg(CurrentXSN).arg(DomNode.lineNumber()));

	PMetaClassTable *MetaTable = new PMetaClassTable(this);
	QString TableName = DomNode.attributes().namedItem("Name").nodeValue();
	QString Parent = DomNode.attributes().namedItem("Parent").nodeValue();
	IS_ASSERT(TableName.length(), QString("Empty table name. File: %1. Line: %2").arg(CurrentXSN).arg(DomNode.lineNumber()));
	IS_ASSERT(!TableName.contains(SYMBOL_SPACE), QString("Forbidden symbol ' ' in table name: %1").arg(TableName));
	MetaTable->setObjectName(TableName);
	MetaTable->SetTypeObject("Table");

	for (int i = 0; i < DomNode.attributes().length(); ++i) //Обход свойств таблицы
	{
		QDomNode Parameter = DomNode.attributes().item(i);
		SetPropertyObject(MetaTable, Parameter.nodeName(), Parameter.nodeValue());
	}

	if (!Parent.length()) //Если мета-таблица не является запросом, проинициализировать системные поля
	{
		InitializeXSNTableSystemFields(MetaTable); //Инициализация системных полей
	}

    InitializeXSNTableSystemFieldsVisible(MetaTable, GetChildDomNode(DomNode, "IncludeSystemFields").firstChild()); //Инициализация видимости системных полей
	InitializeXSNTableFields(MetaTable, GetChildDomNode(DomNode, "Fields").firstChild()); //Инициализация пользовательских полей
	InitializeXSNTableIndexes(MetaTable, GetChildDomNode(DomNode, "Indexes").firstChild()); //Инициализация индексов
	InitializeXSNTableForeigns(MetaTable, GetChildDomNode(DomNode, "Foreigns").firstChild()); //Инициализация внешних ключей
	InitializeXSNTableEscorts(MetaTable, GetChildDomNode(DomNode, "Escorts").firstChild()); //Инициализация эскортов
	InitializeXSNTableJoins(MetaTable, GetChildDomNode(DomNode, "Joins").firstChild()); //Инициализация JOIN'ов

	if (Parent.length()) //Если у мета-таблицы есть родительская таблица - мета-таблица является запросом
	{
		IS_ASSERT(!QueriesMap.contains(TableName), QString("Query \"%1\" already exist in meta data").arg(TableName));
		QueriesMap.insert(TableName, MetaTable);
	}
	else //У мета-таблицы нет родительской таблицы
	{
		//Проверка на заполнение обязательных атрибутов
		IS_ASSERT(MetaTable->GetName().length(), "Empty table name.");
		IS_ASSERT(MetaTable->GetUID().length(), QString("Empty table \"%1\" uid.").arg(MetaTable->GetName()));
		IS_ASSERT(MetaTable->GetAlias().length(), QString("Empty table \"%1\" alias.").arg(MetaTable->GetName()));
		IS_ASSERT(MetaTable->GetLocalName().length(), QString("Empty table \"%1\" local name.").arg(MetaTable->GetName()));
		IS_ASSERT(MetaTable->GetLocalListName().length(), QString("Empty table \"%1\" local list name.").arg(MetaTable->GetName()));
		IS_ASSERT(MetaTable->GetTitleName().length(), QString("Empty table \"%1\" title name.").arg(MetaTable->GetName()));

		IS_ASSERT(!TablesMap.contains(TableName), QString("Table \"%1\" already exist in meta data").arg(TableName));
		TablesMap.insert(TableName, MetaTable);
	}
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
	FieldID->SetIndex(new PMetaClassIndex(true, MetaTable->GetAlias(), MetaTable->GetName(), FieldID->GetName(), FieldID));

	PMetaClassField *FieldUID = MetaTable->GetField("UID");
	IS_ASSERT(FieldUID, "Null field object");
	FieldUID->SetIndex(new PMetaClassIndex(true, MetaTable->GetAlias(), MetaTable->GetName(), FieldUID->GetName(), FieldUID));
	
	PMetaClassField *FieldIsDeleted = MetaTable->GetField("IsDeleted");
	IS_ASSERT(FieldIsDeleted, "Null field object");
	FieldIsDeleted->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldIsDeleted->GetName(), FieldIsDeleted));

	PMetaClassField *FieldIsSystem = MetaTable->GetField("IsSystem");
	IS_ASSERT(FieldIsSystem, "Null field object");
	FieldIsSystem->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldIsSystem->GetName(), FieldIsSystem));

	PMetaClassField *FieldCreationDate = MetaTable->GetField("CreationDate");
	IS_ASSERT(FieldCreationDate, "Null field object");
	FieldCreationDate->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldCreationDate->GetName(), FieldCreationDate));

	PMetaClassField *FieldUpdationDate = MetaTable->GetField("UpdationDate");
	IS_ASSERT(FieldUpdationDate, "Null field object");
	FieldUpdationDate->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldUpdationDate->GetName(), FieldUpdationDate));

	PMetaClassField *FieldDeletionDate = MetaTable->GetField("DeletionDate");
	IS_ASSERT(FieldDeletionDate, "Null field object");
	FieldDeletionDate->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldDeletionDate->GetName(), FieldDeletionDate));

	PMetaClassField *FieldCreationUser = MetaTable->GetField("CreationUser");
	IS_ASSERT(FieldCreationUser, "Null field object");
	FieldCreationUser->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldCreationUser->GetName(), FieldCreationUser));

	PMetaClassField *FieldUpdationUser = MetaTable->GetField("UpdationUser");
	IS_ASSERT(FieldUpdationUser, "Null field object");
	FieldUpdationUser->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldUpdationUser->GetName(), FieldUpdationUser));

	PMetaClassField *FieldDeletionUser = MetaTable->GetField("DeletionUser");
	IS_ASSERT(FieldDeletionUser, "Null field object");
	FieldDeletionUser->SetIndex(new PMetaClassIndex(false, MetaTable->GetAlias(), MetaTable->GetName(), FieldDeletionUser->GetName(), FieldDeletionUser));
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSNTableSystemFieldsVisible(PMetaClassTable *MetaTable, const QDomNode &DomNode)
{
	QDomNode Temp = DomNode;
	while (!Temp.isNull()) //Обход полей
	{
		IS_ASSERT(Temp.attributes().length(), QString("Empty attributes system field. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));
		
		QString FieldName = Temp.attributes().namedItem("Name").nodeValue();
		IS_ASSERT(FieldName.length(), QString("Empty system field name. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));

		PMetaClassField *MetaField = MetaTable->GetField(FieldName);
		IS_ASSERT(MetaField, QString("Not found visible system field %1. File: %2. Line: %3").arg(FieldName).arg(CurrentXSN).arg(Temp.lineNumber()));

		QString LocalName = Temp.attributes().namedItem("LocalName").nodeValue();
		if (LocalName.length())
		{
			MetaField->SetLabelName(LocalName);
			MetaField->SetLocalListName(LocalName);
		}

		MetaTable->AddVisibleSystemField(MetaField);
		Temp = Temp.nextSibling();
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSNTableFields(PMetaClassTable *MetaTable, const QDomNode &DomNode)
{
	QDomNode Temp = DomNode;
	while (!Temp.isNull()) //Обход полей
	{
		if (Temp.isComment()) //Если обнаружен комментарий - переходить на следующий НОД
		{
			Temp = Temp.nextSibling();
			continue;
		}
		else
		{
			IS_ASSERT(Temp.attributes().length(), QString("Empty attributes field. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));
		}
		
		PMetaClassField *MetaField = new PMetaClassField(MetaTable);
		IS_ASSERT(Temp.nodeName() == "Field", QString("Invalid field tag name: %1").arg(Temp.nodeName()));
		QString FieldName = Temp.attributes().namedItem("Name").nodeValue();
		IS_ASSERT(FieldName.length(), QString("Empty field name. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));
		IS_ASSERT(!FieldName.contains(SYMBOL_SPACE), QString("Forbidden symbol ' ' in field name: %1").arg(FieldName));
		MetaField->setObjectName(FieldName);
		MetaField->SetTypeObject("Field");

		for (int i = 0; i < Temp.attributes().length(); ++i) //Обход свойств поля
		{
			QDomNode Parameter = Temp.attributes().item(i);
			SetPropertyObject(MetaField, Parameter.nodeName(), Parameter.nodeValue());
		}

		if (MetaField->GetIsSystem()) //Если поле системное
		{
			MetaTable->AddSystemField(MetaField);
		}
		else //Поле стандартное
		{
			MetaTable->AddField(MetaField);
		}

		if (!MetaTable->GetParent().length() && !MetaField->GetQueryText().length())
		{
			//Проверка на заполнение обязательных атрибутов
			IS_ASSERT(MetaField->GetUID().length(), QString("Empty field uid."));
			IS_ASSERT(MetaField->GetName().length(), QString("Empty field name."));
			IS_ASSERT(MetaField->GetLabelName().length(), QString("Empty field \"%1\" label name.").arg(MetaField->GetUID()));
			IS_ASSERT(MetaField->GetLocalListName().length(), QString("Empty field \"%1\" local list name.").arg(MetaField->GetUID()));
		}

		IS_ASSERT(!MetaTable->GetField(FieldName), QString("Field \"%1\" already exist. TableName: %2").arg(FieldName).arg(MetaTable->GetName())); //Проверка на уникальность поля в мета-таблице
		MetaTable->AddAllField(MetaField);
		Temp = Temp.nextSibling();
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSNTableIndexes(PMetaClassTable *MetaTable, const QDomNode &DomNode)
{
	QDomNode Temp = DomNode;
	while (!Temp.isNull()) //Обход индексов
	{
		if (Temp.isComment()) //Если обнаружен комментарий - переходить на следующий НОД
		{
			Temp = Temp.nextSibling();
			continue;
		}
		else
		{
			IS_ASSERT(Temp.attributes().length(), QString("Empty attributes index. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));
		}

		IS_ASSERT(Temp.nodeName() == "Index", QString("Invalid index tag name: %1").arg(Temp.nodeName()));
		QString FieldName = Temp.attributes().namedItem("Field").nodeValue();
		IS_ASSERT(FieldName.length(), QString("Empty index name. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));

		bool Unique = QVariant(Temp.attributes().namedItem("Unique").nodeValue()).toBool();
		QStringList IndexList = FieldName.split(';');

		if (IndexList.count() > 1) //Если индекс составной
		{
			PMetaClassIndex *Index = new PMetaClassIndex(Unique, MetaTable->GetAlias(), MetaTable->GetName(), QString(), this);
			for (const QString &IndexName : IndexList)
			{
				Index->AddField(IndexName);
			}
			IndexesCompound.append(Index);
		}
		else //Индекс стандартный
		{
			PMetaClassField *MetaField = MetaTable->GetField(FieldName);
			IS_ASSERT(MetaField, QString("Not found field from name: %1").arg(FieldName));
			IS_ASSERT(!MetaField->GetIndex(), QString("Index already exist. TableName: %1. FieldName: %2.").arg(MetaTable->GetName()).arg(FieldName)); //Проверка индекса на уникальность
			MetaField->SetIndex(new PMetaClassIndex(Unique, MetaTable->GetAlias(), MetaTable->GetName(), FieldName, MetaField));
		}

		Temp = Temp.nextSibling();
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSNTableForeigns(PMetaClassTable *MetaTable, const QDomNode &DomNode)
{
	QDomNode Temp = DomNode;
	while (!Temp.isNull())
	{
		IS_ASSERT(Temp.attributes().length(), QString("Empty attributes foreign. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));

		PMetaClassForeign *MetaForeign = new PMetaClassForeign(this);
		IS_ASSERT(Temp.nodeName() == "Foreign", QString("Invalid foreign tag name: %1").arg(Temp.nodeName()));
		QString FieldName = Temp.attributes().namedItem("Field").nodeValue();
		IS_ASSERT(MetaTable->GetField(FieldName), QString("Not found field \"%1\" in table \"%2\"").arg(FieldName).arg(MetaTable->GetName()));
		IS_ASSERT(FieldName.length(), QString("Empty foreign field. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));
		MetaForeign->SetTypeObject("Foreign");

		for (int i = 0; i < Temp.attributes().length(); ++i)
		{
			QDomNode Parameter = Temp.attributes().item(i);
			SetPropertyObject(MetaForeign, Parameter.nodeName(), Parameter.nodeValue());
		}

		MetaForeign->SetTableName(MetaTable->GetName());

		PMetaClassField *MetaField = MetaTable->GetField(FieldName);
		IS_ASSERT(MetaField, QString("Not found field \"%1\" in table \"%2\"").arg(FieldName).arg(MetaTable->GetName())); //Проверка поля на существование
		IS_ASSERT(!MetaField->GetForeign(), QString("Foreign already exist. TableName: %1. FieldName: %2.").arg(MetaTable->GetName()).arg(FieldName)); //Проверка внешнего ключа на уникальность
		MetaField->SetForeign(MetaForeign);

		Temp = Temp.nextSibling();
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSNTableEscorts(PMetaClassTable *MetaTable, const QDomNode &DomNode)
{
	QDomNode Temp = DomNode;
	while (!Temp.isNull())
	{
		IS_ASSERT(Temp.attributes().length(), QString("Empty attributes escort. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));

		PMetaClassEscort *MetaEscort = new PMetaClassEscort(MetaTable);
		MetaEscort->SetTypeObject("Escort");

		for (int i = 0; i < Temp.attributes().length(); ++i)
		{
			QDomNode Parameter = Temp.attributes().item(i);
			SetPropertyObject(MetaEscort, Parameter.nodeName(), Parameter.nodeValue());
		}

		MetaTable->AddEscort(MetaEscort);
		Temp = Temp.nextSibling();
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSNTableJoins(PMetaClassTable *MetaTable, const QDomNode &DomNode)
{
	QDomNode Temp = DomNode;
	while (!Temp.isNull())
	{
		IS_ASSERT(Temp.attributes().length(), QString("Empty attributes field. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));

		QString JoinText = Temp.attributes().namedItem("Text").nodeValue();
		IS_ASSERT(JoinText.length(), QString("Empty join text. File: %1. Line: %2").arg(CurrentXSN).arg(Temp.lineNumber()));

		MetaTable->AddJoin(JoinText);
		Temp = Temp.nextSibling();
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSR()
{
	QStringList Filter("*.xsr");
	QFileInfoList FileInfoList;
	FileInfoList.append(QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка ресурсов движка

	if (Configuration.length())
	{
		FileInfoList.append(QDir(":_" + Configuration).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка ресурсов конфигурации
	}

	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех XSR файлов
	{
		QFile FileXSR(FileInfo.filePath());
		IS_ASSERT(FileXSR.open(QIODevice::ReadOnly), QString("Not opened xsr file: %1").arg(FileInfo.filePath()));

		QString Content = FileXSR.readAll();
		FileXSR.close();

		InitializeXSR(Content);
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSR(const QString &Content)
{
	ISCountingTime Time;
	QDomElement DomElement = ISSystem::GetDomElement(Content);

	QString TagName = DomElement.tagName();
	IS_ASSERT(TagName.length(), "Empty tag name");
	IS_ASSERT(TagName == "XSR", QString("Invalid tag name \"%1\" in XSR file").arg(TagName));
	
	QString SchemeName = DomElement.attributes().namedItem("Name").nodeValue();
	IS_ASSERT(SchemeName.length(), "Empty schema name");

	CurrentXSR = SchemeName;

	QDomNode DomNode = DomElement.firstChild();
	while (!DomNode.isNull()) //Обход ресурсов
	{
		if (DomNode.isComment())
		{
			DomNode = DomNode.nextSibling();
			continue;
		}

		QDomNamedNodeMap DomNodeMap = DomNode.attributes();
		IS_ASSERT(DomNodeMap.length(), QString("Empty resource arguments. File: %1. Line: %2").arg(CurrentXSR).arg(DomNode.lineNumber()));

		QString TableName = DomNode.nodeName();
		IS_ASSERT(TableName.length(), QString("Empty resource name. File: %1. Line: %2").arg(CurrentXSR).arg(DomNode.lineNumber()));

		PMetaClassResource *MetaResource = new PMetaClassResource(this);
		MetaResource->SetTableName(TableName);

		for (int i = 0; i < DomNodeMap.length(); ++i) //Обход полей ресурса
		{
			QDomNode DomItem = DomNodeMap.item(i); // Поле-значение

			QString FieldName = DomItem.nodeName(); //Имя поля
			QString Value = DomItem.nodeValue(); //Значение поля

			PMetaClassTable *MetaTable = TablesMap.value(TableName);
			IS_ASSERT(MetaTable, QString("Invalid table name \"%1\" in resource. File: %2. Line: %3").arg(TableName).arg(CurrentXSR).arg(DomNode.lineNumber()));

			PMetaClassField *MetaField = MetaTable->GetField(FieldName);
			IS_ASSERT(MetaField, QString("Invalid field name \"%1\" in resource. File: %2. Line: %3").arg(FieldName).arg(CurrentXSR).arg(DomNode.lineNumber()));

			MetaResource->AddField(FieldName, Value);
		}

		MetaResource->SetUID(ISUuid(MetaResource->GetUID()));
		IS_ASSERT(MetaResource->GetUID().length(), QString("Empty UID in resource. File: %1. Line: %2").arg(CurrentXSR).arg(DomNode.lineNumber()));

		Resources.append(MetaResource);
		DomNode = DomNode.nextSibling();
	}

	CurrentXSR.clear();
	ISDebug::ShowDebugString(QString("Init XSR %1: %2 msec").arg(SchemeName).arg(Time.GetElapsed()));
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSF()
{
	QStringList Filter("*.xsf");
	QFileInfoList FileInfoList;
	FileInfoList.append(QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка функций движка

	if (Configuration.length())
	{
		FileInfoList.append(QDir(":_" + Configuration).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка функций конфигурации
	}

	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех XSF файлов
	{
		QFile FileXSF(FileInfo.filePath());
		IS_ASSERT(FileXSF.open(QIODevice::ReadOnly), QString("Not opened xsf file: %1").arg(FileInfo.filePath()));

		QString Content = FileXSF.readAll();
		FileXSF.close();

		InitializeXSF(Content);
	}
}
//-----------------------------------------------------------------------------
void ISMetaData::InitializeXSF(const QString &Content)
{
	ISCountingTime Time;
	QDomElement DomElement = ISSystem::GetDomElement(Content);

	QString TagName = DomElement.tagName();
	IS_ASSERT(TagName.length(), "Empty tag name");
	IS_ASSERT(TagName == "XSF", QString("Invalid tag name \"%1\" in XSF file.").arg(TagName));

	QString SchemeName = DomElement.attributes().namedItem("Name").nodeValue();
	IS_ASSERT(SchemeName.length(), "Empty schema name");

	CurrentXSF = SchemeName;

	QDomNode DomNode = DomElement.firstChild();
	while (!DomNode.isNull()) //Обход элементов
	{
		QDomNamedNodeMap DomNodeMap = DomNode.attributes();
		IS_ASSERT(DomNodeMap.length(), QString("Empty element arguments. File: %1. Line: %2").arg(CurrentXSF).arg(DomNode.lineNumber()));

		QString ElementName = DomNode.nodeName();
		IS_ASSERT(ElementName.length(), QString("Empty element name. File: %1. Line: %2").arg(CurrentXSR).arg(DomNode.lineNumber()));

		if (ElementName == "Function")
		{
			PMetaClassFunction *MetaFunction = new PMetaClassFunction(this);
			QString FunctionName = DomNode.attributes().namedItem("Name").nodeValue();
			MetaFunction->SetTypeObject(ElementName);

			for (int i = 0; i < DomNode.attributes().length(); ++i)
			{
				QDomNode Parameter = DomNode.attributes().item(i);
				SetPropertyObject(MetaFunction, Parameter.nodeName(), Parameter.nodeValue());
			}
			
			FunctionsMap.insert(FunctionName, MetaFunction);
		}

		DomNode = DomNode.nextSibling();
	}

	CurrentXSF.clear();
	ISDebug::ShowDebugString(QString("Init XSF %1: %2 msec").arg(SchemeName).arg(Time.GetElapsed()));
}
//-----------------------------------------------------------------------------
void ISMetaData::SetPropertyObject(PMetaClass *MetaClass, const QString &PropertyName, const QVariant &PropertyValue)
{
	const QMetaObject *MetaObject = MetaClass->metaObject();
	for (int i = 0; i < MetaObject->propertyCount(); ++i)
	{
		QMetaProperty MetaProperty = MetaObject->property(i);
		const char *Name = MetaProperty.name();
		if (Name == PropertyName)
		{
			IS_ASSERT(MetaClass->setProperty(PropertyName.toUtf8().data(), PropertyValue), QString("Not changed property: %1. TypeObjec: %2. ObjectName: %3. File: %4").arg(PropertyName).arg(MetaClass->GetTypeObject()).arg(MetaClass->objectName()).arg(CurrentXSN));
			return;
		}
	}

	IS_ASSERT(false, QString("Not found property: %1. TypeObject: %2. ObjectName: %3. File: %4").arg(PropertyName).arg(MetaClass->GetTypeObject()).arg(MetaClass->objectName()).arg(CurrentXSN));
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
