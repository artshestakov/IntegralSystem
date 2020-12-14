#include "CGDatabase.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISSystem.h"
#include "ISConsole.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
static QString QS_FOREIGN = PREPARE_QUERY("SELECT COUNT(*) "
										  "FROM information_schema.constraint_table_usage "
										  "WHERE table_catalog = current_database() "
										  "AND table_schema = current_schema() "
										  "AND constraint_name = :ForeignName");
//-----------------------------------------------------------------------------
static QString QC_FOREIGN = "ALTER TABLE public.%1 "
							"ADD CONSTRAINT %2 FOREIGN KEY (%3) "
							"REFERENCES public.%4(%5_id) "
							"ON DELETE NO ACTION "
							"ON UPDATE NO ACTION "
							"NOT DEFERRABLE";
//-----------------------------------------------------------------------------
static QString QS_FUNCTION = PREPARE_QUERY("SELECT proname || '(' || pg_get_function_arguments(oid) || ')' AS function_name "
										   "FROM pg_proc "
										   "WHERE pronamespace = (SELECT oid FROM pg_namespace WHERE nspname = current_schema()) "
										   "AND probin IS NULL "
										   "ORDER BY function_name");
//-----------------------------------------------------------------------------
static QString QD_FOREIGN = "ALTER TABLE public.%1 DROP CONSTRAINT %2 RESTRICT";
//-----------------------------------------------------------------------------
static QString QS_COLUMN = PREPARE_QUERY("SELECT COUNT(*) "
										 "FROM information_schema.columns "
										 "WHERE table_catalog = current_database() "
										 "AND table_schema = current_schema() "
										 "AND table_name = :TableName "
										 "AND column_name = :ColumnName");
//-----------------------------------------------------------------------------
static QString QS_INDEXES = PREPARE_QUERY("SELECT COUNT(*) FROM pg_indexes WHERE schemaname = current_schema() AND tablename = :TableName AND indexname = :IndexName");
//-----------------------------------------------------------------------------
static QString QC_INDEX = "CREATE %1 INDEX %2 ON public.%3 USING btree(%4)";
//-----------------------------------------------------------------------------
static QString QS_INDEX = PREPARE_QUERY("SELECT indisunique "
										"FROM pg_indexes "
										"JOIN pg_class c ON c.relname = indexname "
										"JOIN pg_index ON indexrelid = c.oid "
										"WHERE schemaname = current_schema() "
										"AND indexname = :IndexName");
//-----------------------------------------------------------------------------
static QString QD_INDEX = PREPARE_QUERY("DROP INDEX public.%1");
//-----------------------------------------------------------------------------
static QString Q_REINDEX = "REINDEX INDEX %1";
//-----------------------------------------------------------------------------
static QString QS_RESOURCE = "SELECT COUNT(*) FROM %1 WHERE %2_uid = :ResourceUID";
//-----------------------------------------------------------------------------
static QString QU_RESET_RESOURCE_FIELD = "UPDATE %1 SET %2 = NULL WHERE %3_uid = :ResourceUID";
//-----------------------------------------------------------------------------
static QString QU_RESOURCE_FIELD = "UPDATE %1 SET %2 = :ResourceValue WHERE %3_uid = :ResourceUID";
//-----------------------------------------------------------------------------
static QString QS_SEQUENCES = PREPARE_QUERY("SELECT COUNT(*) "
											"FROM information_schema.sequences "
											"WHERE sequence_catalog = current_database() "
											"AND sequence_name = :SequenceName");
//-----------------------------------------------------------------------------
static QString QC_SEQUENCE = "CREATE SEQUENCE public.%1 "
							 "INCREMENT 1 MINVALUE 1 "
							 "MAXVALUE 2147483647 START 1 "
							 "CACHE 1";
//-----------------------------------------------------------------------------
static QString QS_TABLE = PREPARE_QUERY("SELECT COUNT(*) FROM pg_tables WHERE schemaname = current_schema() AND tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QS_COLUMNS = PREPARE_QUERY("SELECT column_name AS column_full_name, split_part(column_name, '_', 2) AS column_name, column_default, is_nullable::BOOLEAN, data_type, character_maximum_length "
										  "FROM information_schema.columns "
										  "WHERE table_catalog = current_database() "
										  "AND table_schema = current_schema() "
										  "AND table_name = :TableName "
										  "ORDER BY ordinal_position");
//-----------------------------------------------------------------------------
static QString QS_OLD_COLUMNS = PREPARE_QUERY("SELECT "
											  "column_name AS column_full_name, "
											  "substr(column_name, position('_' IN column_name) + 1, length(column_name) - position('_' IN column_name)) AS column_name "
											  "FROM information_schema.columns "
											  "WHERE table_catalog = current_database() "
											  "AND table_schema = current_schema() "
											  "AND table_name = :TableName "
											  "ORDER BY ordinal_position");
//-----------------------------------------------------------------------------
bool CGDatabase::Foreign_Create(PMetaForeign *MetaForeign, QString &ErrorString)
{
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(MetaForeign->TableName);
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass);

	QString SqlText;
	SqlText += "ALTER TABLE public." + MetaTable->Name.toLower() + " \n";
	SqlText += "ADD CONSTRAINT " + MetaForeign->GetName() + " FOREIGN KEY (" + MetaTable->Alias + '_' + MetaForeign->Field.toLower() + ") \n";
	SqlText += "REFERENCES public." + MetaTableForeign->Name.toLower() + '(' + MetaTableForeign->Alias.toLower() + '_' + MetaForeign->ForeignField.toLower() + ") \n";
	SqlText += "ON DELETE CASCADE \n";
	SqlText += "ON UPDATE NO ACTION \n";
	SqlText += "NOT DEFERRABLE;";

	ISQuery qCreateForeign;
	qCreateForeign.SetShowLongQuery(false);
	bool Result = qCreateForeign.Execute(SqlText);
	if (!Result)
	{
		ErrorString = qCreateForeign.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Foreign_Update(PMetaForeign *MetaForeign, QString &ErrorString)
{
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(MetaForeign->TableName);

	ISQuery qDeleteForeign;
	qDeleteForeign.SetShowLongQuery(false);
	bool Result = qDeleteForeign.Execute(QD_FOREIGN.arg(MetaTable->Name.toLower()).arg(MetaForeign->GetName()));
	if (Result)
	{
		Result = Foreign_Create(MetaForeign, ErrorString);
	}
	else
	{
		ErrorString = qDeleteForeign.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Foreign_Exist(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString)
{
	ISQuery qSelect(QS_FOREIGN);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":ForeignName", MetaForeign->GetName());
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		Exist = qSelect.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Function_Create(PMetaFunction *MetaFunction, QString &ErrorString)
{
	ISQuery qCreateFunction;
	qCreateFunction.SetShowLongQuery(false);
	bool Result = qCreateFunction.Execute(MetaFunction->Text);
	if (Result)
	{
		Result = Helper_CommentFunction(MetaFunction, ErrorString);
	}
	else
	{
		ErrorString = qCreateFunction.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Function_Delete(QString &ErrorString)
{
	ISQuery qSelectFunction(QS_FUNCTION);
	qSelectFunction.SetShowLongQuery(false);
	bool Result = qSelectFunction.Execute(), Founded = false;
	if (Result)
	{
		while (qSelectFunction.Next()) //Обходим функции из БД
		{
			QString FunctionName = qSelectFunction.ReadColumn("function_name").toString();

			//Ищем такую функцию в мета-данных
			for (PMetaFunction *MetaFunction : ISMetaData::Instance().GetFunctions())
			{
				if (MetaFunction->Name.toLower() == FunctionName) //Функций найдена - выходим из цикла
				{
					Founded = true;
					break;
				}
			}

			if (!Founded) //Если функция не найдена - предлагаем удалить
			{
				if (ISConsole::Question(QString("Delete old function \"%1\"?").arg(FunctionName))) //Согласились на удаление
				{
					ISQuery qDelete;
					qDelete.SetShowLongQuery(false);
					Result = qDelete.Execute("DROP FUNCTION " + FunctionName);
					if (!Result)
					{
						ErrorString = qDelete.GetErrorString();
						break;
					}
				}
			}
			Founded = false; //Возвращаем значение флага в исходное положение
		}
	}
	else
	{
		ErrorString = qSelectFunction.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Index_Create(PMetaIndex *Index, QString &ErrorString)
{
	QString Fields, SqlText = QC_INDEX.arg(Index->Unique ? "UNIQUE" : QString()).arg(Index->GetName()).arg(Index->TableName);
	if (!Index->Fields.empty())
	{
		for (const QString &String : Index->Fields)
		{
			Fields += (Index->Alias + '_' + String).toLower() + ", ";
		}
		Fields.chop(2);
		SqlText = SqlText.arg(Fields);
	}
	else
	{
		SqlText = SqlText.arg(Index->Alias + '_' + Index->FieldName.toLower());
	}

	ISQuery qCreateIndex;
	qCreateIndex.SetShowLongQuery(false);
	bool Result = qCreateIndex.Execute(SqlText);
	if (!Result)
	{
		ErrorString = qCreateIndex.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Index_Update(PMetaIndex *Index, QString &ErrorString)
{
	ISQuery qSelect(QS_INDEX);
	qSelect.BindValue(":IndexName", Index->GetName());
	qSelect.SetShowLongQuery(false);
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		//Если уникальность индекса в БД не совпадает с мета-данными - обновляем
		if (qSelect.ReadColumn("indisunique").toBool() != Index->Unique)
		{
			Result = Index_Delete(Index, ErrorString);
			if (Result)
			{
				Result = Index_Create(Index, ErrorString);
			}
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Index_Delete(PMetaIndex *Index, QString &ErrorString)
{
	ISQuery qDelete;
	bool Result = qDelete.Execute(QD_INDEX.arg(Index->GetName()));
	if (!Result)
	{
		ErrorString = qDelete.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Index_Exist(PMetaIndex *Index, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectIndex(QS_INDEXES);
	qSelectIndex.SetShowLongQuery(false);
	qSelectIndex.BindValue(":TableName", Index->TableName.toLower());
	qSelectIndex.BindValue(":IndexName", Index->GetName());
	bool Result = qSelectIndex.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectIndex.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectIndex.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Index_CheckForeign(PMetaIndex *Index)
{
	bool Result = true;
	std::vector<PMetaForeign*> Foreigns = ISMetaData::Instance().GetForeigns();
	for (PMetaForeign *MetaForeign : Foreigns)
	{
		Result = Index->TableName.toLower() == MetaForeign->ForeignClass.toLower();
		if (Result)
		{
			Result = Index->FieldName.toLower() == MetaForeign->ForeignField.toLower();
			if (Result)
			{
				break;
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Index_ReIndex(PMetaIndex *Index, QString &ErrorString)
{
	ISQuery qReindex;
	qReindex.SetShowLongQuery(false);
	bool Result = qReindex.Execute(Q_REINDEX.arg(Index->GetName()));
	if (!Result)
	{
		ErrorString = qReindex.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_Insert(PMetaResource *MetaResource, QString &ErrorString)
{
	QString TableAlias = ISMetaData::Instance().GetMetaTable(MetaResource->TableName)->Alias;

	QString InsertText = "INSERT INTO " + MetaResource->TableName.toLower() + '(' + TableAlias + "_uid, ";
	QString ValuesText = "VALUES(:UID, ";

	size_t CountParameters = MetaResource->Parameters.size();
	for (size_t i = 0; i < CountParameters; ++i)
	{
		QString FieldName = TableAlias + '_' + ISAlgorithm::ConvertMapToKeys(MetaResource->Parameters)[i].toLower();
		InsertText += FieldName + ", ";
		ValuesText += ':' + FieldName + ", ";
	}

	InsertText.chop(2);
	InsertText += ") \n";

	ValuesText.chop(2);
	ValuesText += ");";

	ISQuery qInsertResource(InsertText + ValuesText);
	qInsertResource.SetShowLongQuery(false);

	bool BindValueUID = qInsertResource.BindValue(":UID", MetaResource->UID);
	IS_ASSERT(BindValueUID, QString("Not BindValue Field UID. UID: %1").arg(MetaResource->UID));

	for (size_t i = 0; i < CountParameters; ++i)
	{
		QString FieldName = TableAlias + '_' + ISAlgorithm::ConvertMapToKeys(MetaResource->Parameters)[i].toLower();
		QString FieldValue = ISAlgorithm::ConvertMapToValues(MetaResource->Parameters)[i];
		bool BindValue = qInsertResource.BindValue(':' + FieldName, FieldValue);
		IS_ASSERT(BindValue, QString("Not BindValue. TableName: %1. FieldName: %2").arg(MetaResource->TableName).arg(FieldName));
	}

	bool Result = qInsertResource.Execute();
	if (!Result)
	{
		ErrorString = qInsertResource.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_Update(PMetaResource *MetaResource, QString &ErrorString)
{
	QString TableName = MetaResource->TableName;
	QString TableAlias = ISMetaData::Instance().GetMetaTable(TableName)->Alias;
	QString ResourceUID = MetaResource->UID;

	bool Result = Resource_UpdateField(TableName, TableAlias, TableAlias + "_issystem", false, ResourceUID, ErrorString);
	IS_ASSERT(Result, QString("Error update resource field. TableName: %1 FieldName: %2 UID: %3 Error: %4").arg(TableName).arg(TableAlias + "_issystem").arg(ResourceUID).arg(ErrorString));

	for (const auto &Resource : MetaResource->Parameters) //Обход параметров ресурса
	{
		QString FieldName = Resource.first;
		QString FieldNameComplete = QString(TableAlias + '_' + Resource.first).toLower();
		QString FieldValue = Resource.second;

		PMetaField *MetaField = ISMetaData::Instance().GetMetaField(TableName, FieldName);
		if (!MetaField->NotNull) //Если поле НЕ ЯВЛЯЕТСЯ обязательным для заполнения - обнулить его
		{
			bool ResetField = Resource_ResetField(TableName, TableAlias, FieldNameComplete, ResourceUID, ErrorString);
			IS_ASSERT(ResetField, QString("Not reset resource field. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(FieldNameComplete).arg(ResourceUID).arg(ErrorString));
		}

		Result = Resource_UpdateField(TableName, TableAlias, FieldNameComplete, FieldValue, ResourceUID, ErrorString);
		IS_ASSERT(Result, QString("Not update resource. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(FieldNameComplete).arg(ResourceUID).arg(ErrorString));
	}

	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableName);
	for (PMetaField *MetaField : MetaTable->Fields) //Обход пользовательских полей таблицы и их очистка
	{
		if (MetaField->NotNull || !MetaField->QueryText.isEmpty() || MetaField->IsSystem)
		{
			continue;
		}

		if (!ISAlgorithm::VectorContains(ISAlgorithm::ConvertMapToKeys(MetaResource->Parameters), MetaField->Name))
		{
			bool ResetUserField = Resource_ResetField(TableName, TableAlias, TableAlias + '_' + MetaField->Name, ResourceUID, ErrorString);
			IS_ASSERT(ResetUserField, QString("Not reset resource field. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(TableAlias + '_' + MetaField->Name).arg(ResourceUID).arg(ErrorString));
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_Exist(PMetaResource *MetaResource, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectResource(QS_RESOURCE.arg(MetaResource->TableName).arg(ISMetaData::Instance().GetMetaTable(MetaResource->TableName)->Alias));
	qSelectResource.SetShowLongQuery(false);
	qSelectResource.BindValue(":ResourceUID", MetaResource->UID);
	bool Result = qSelectResource.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectResource.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectResource.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_ResetField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString)
{
	ISQuery qReset(QU_RESET_RESOURCE_FIELD.arg(TableName).arg(FieldName).arg(TableAlias));
	qReset.SetShowLongQuery(false);
	qReset.BindValue(":ResourceUID", ResourceUID);
	bool Result = qReset.Execute();
	if (!Result)
	{
		ErrorString = qReset.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_UpdateField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString)
{
	ISQuery qUpdate(QU_RESOURCE_FIELD.arg(TableName).arg(FieldName).arg(TableAlias));
	qUpdate.SetShowLongQuery(false);
	qUpdate.BindValue(":ResourceValue", Value);
	qUpdate.BindValue(":ResourceUID", ResourceUID);
	bool Result = qUpdate.Execute();
	if (!Result)
	{
		ErrorString = qUpdate.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Sequence_Create(const QString &TableName, QString &ErrorString)
{
	ISQuery qCreateSequence;
	qCreateSequence.SetShowLongQuery(false);
	bool Result = qCreateSequence.Execute(QC_SEQUENCE.arg(Sequence_GetName(TableName)));
	if (!Result)
	{
		ErrorString = qCreateSequence.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Sequence_Exist(const QString &TableName, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectSequences(QS_SEQUENCES);
	qSelectSequences.SetShowLongQuery(false);
	qSelectSequences.BindValue(":SequenceName", Sequence_GetName(TableName));
	bool Result = qSelectSequences.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectSequences.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectSequences.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString CGDatabase::Sequence_GetName(const QString &TableName)
{
	return TableName + "_sequence";
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_Create(PMetaTable *MetaTable, QString &ErrorString)
{
	QString TableName = MetaTable->Name.toLower(), TableAlias = MetaTable->Alias.toLower();

	bool Exist = true;
	bool Result = CGDatabase::Sequence_Exist(TableName, Exist, ErrorString);
	if (Result)
	{
		if (!Exist)
		{
			Result = CGDatabase::Sequence_Create(TableName, ErrorString);
		}
	}

	if (!Result)
	{
		return Result;
	}

	//Заголовок запроса
	QString SqlText = "CREATE TABLE public." + MetaTable->Name.toLower() + " \n( \n";

	//Формирование запроса на создание таблицы
	for (PMetaField *MetaField : MetaTable->Fields) //Обход полей таблицы
	{
		if (!MetaField->QueryText.isEmpty())
		{
			continue;
		}

		QString FieldName = MetaField->Name; //Имя поля
		ISNamespace::FieldType FieldType = MetaField->Type; //Тип поля
		int FieldSize = MetaField->Size; //Размер поля
		QString FieldDefalutValue = MetaField->DefaultValue.toString(); //Значение по умолчанию для поля
		bool FieldNotNull = MetaField->NotNull; //Статус обязательного заполнения поля

		SqlText += "\t" + TableAlias + '_' + FieldName.toLower() + SYMBOL_SPACE + ISMetaData::Instance().GetTypeDB(FieldType);

		if (FieldSize > 0) //Если указан размер поля
		{
			SqlText += QString("(%1)").arg(FieldSize);
		}

		if (MetaField->Sequence) //Если поле является последовательностью - устанавливаем счётчик значением по умолчанию
		{
			SqlText += QString(" DEFAULT nextval('%1'::regclass)").arg(CGDatabase::Sequence_GetName(TableName));
		}
		else //Поле не является поледовательностью - устанавливаем указанное значение по умолчанию
		{
			//Если значние по умолчанию не указано - ставим NULL, иначе - то что указали
			SqlText += " DEFAULT " + (FieldDefalutValue.isEmpty() ? "NULL" : FieldDefalutValue);
		}

		if (FieldNotNull) //Если поле обязательно для заполнения
		{
			SqlText += " NOT NULL";
		}

		if (MetaField->PrimaryKey) //Если поле является первичным ключом
		{
			SqlText += " PRIMARY KEY";
		}
		SqlText += ",\n";
	}

	//Завершение формирования запроса на создание таблицы
	SqlText.chop(2);
	SqlText += "\n);";

	//Исполнение запроса
	ISQuery qCreateTable;
	qCreateTable.SetShowLongQuery(false);
	Result = qCreateTable.Execute(SqlText);
	if (!Result)
	{
		ErrorString = qCreateTable.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_Update(PMetaTable *MetaTable, QString &ErrorString)
{
	bool Result = Table_AlterFields(MetaTable, ErrorString);
	if (Result)
	{
		Result = Table_CreateFields(MetaTable, ErrorString);
	}

	if (Result)
	{
		Result = Table_DeleteFields(MetaTable, ErrorString);
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_Exist(PMetaTable *MetaTable, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectTable(QS_TABLE);
	qSelectTable.SetShowLongQuery(false);
	qSelectTable.BindValue(":TableName", MetaTable->Name.toLower());
	bool Result = qSelectTable.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectTable.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectTable.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_AlterFields(PMetaTable *MetaTable, QString &ErrorString)
{
	ISQuery qSelectColumns(QS_COLUMNS);
	qSelectColumns.SetShowLongQuery(false);
	qSelectColumns.BindValue(":TableName", MetaTable->Name.toLower());
	bool Result = qSelectColumns.Execute();
	if (Result)
	{
		while (qSelectColumns.Next())
		{
			QString ColumnNameFull = qSelectColumns.ReadColumn("column_full_name").toString();
			QString ColumnName = qSelectColumns.ReadColumn("column_name").toString();
			QString ColumnDefaultValue = qSelectColumns.ReadColumn("column_default").toString();
			bool ColumnNotNull = !qSelectColumns.ReadColumn("is_nullable").toBool();
			QString ColumnType = qSelectColumns.ReadColumn("data_type").toString().toUpper();
			int ColumnSize = qSelectColumns.ReadColumn("character_maximum_length").toInt();

			if (!ISMetaData::Instance().CheckExitField(MetaTable, ColumnName))
			{
				continue;
			}

			PMetaField *MetaField = ISMetaData::Instance().GetMetaField(MetaTable, ColumnName);
			QString MetaType = ISMetaData::Instance().GetTypeDB(MetaField->Type);
			QString MetaDefaultValue = MetaField->DefaultValue.toString();
			bool MetaNotNull = MetaField->NotNull;
			int MetaSize = MetaField->Size;

			//Проверка соответствия типов
			if (ColumnType != MetaType || ColumnSize != MetaSize)
			{
				QString QueryText = "ALTER TABLE public." + MetaTable->Name.toLower() + " \n";
				QueryText += "ALTER COLUMN " + MetaTable->Alias + '_' + MetaField->Name.toLower() + " TYPE " + MetaType;
				QueryText += MetaSize > 0 ? QString("(%1) \n").arg(MetaSize) : " \n";
				QueryText += "USING " + MetaTable->Alias + '_' + MetaField->Name.toLower() + "::" + MetaType;

				ISQuery qAlterType;
				qAlterType.SetShowLongQuery(false);
				Result = qAlterType.Execute(QueryText);
				if (!Result)
				{
					ErrorString = qAlterType.GetErrorString();
					break;
				}
			}

			//Если поле является ID - то не продолжаем дальше
			if (ColumnName == "id")
			{
				continue;
			}

			//Проверка соответствия значения по умолчанию
			if (ColumnDefaultValue != MetaDefaultValue)
			{
				//Если в мета-данных значение по умолчанию не указано - удаляем текущее (даже если его нет), иначе - изменяем
				QString QueryText = "ALTER TABLE public." + MetaTable->Name +
					" ALTER COLUMN " + ColumnNameFull + (MetaDefaultValue.isEmpty() ? " DROP DEFAULT" : (" SET DEFAULT " + MetaDefaultValue));
				ISQuery qAlterColumn;
				qAlterColumn.SetShowLongQuery(false);
				Result = qAlterColumn.Execute(QueryText);
				if (!Result)
				{
					ErrorString = qAlterColumn.GetErrorString();
					break;
				}
			}

			//Проверка соответствия флага обязательного заполнения
			if (ColumnNotNull != MetaNotNull)
			{
				QString QueryText;
				if (!ColumnNotNull && MetaNotNull)
				{
					QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN " + MetaTable->Alias + '_' + MetaField->Name + " SET NOT NULL";
				}
				else if (ColumnNotNull && !MetaNotNull)
				{
					QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN " + MetaTable->Alias + '_' + MetaField->Name + " DROP NOT NULL";
				}

				ISQuery qAlterNotNull;
				qAlterNotNull.SetShowLongQuery(false);
				Result = qAlterNotNull.Execute(QueryText);
				if (!Result)
				{
					ErrorString = qAlterNotNull.GetErrorString();
					break;
				}
			}
		}
	}
	else
	{
		ErrorString = qSelectColumns.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_CreateFields(PMetaTable *MetaTable, QString &ErrorString)
{
	bool Result = true, Exist = true;
	for (PMetaField *MetaField : MetaTable->Fields) //Обход полей
	{
		if (!MetaField->QueryText.isEmpty())
		{
			continue;
		}

		QString FieldName = MetaTable->Alias + '_' + MetaField->Name.toLower();
		if (CGDatabase::Helper_ExistField(MetaTable, FieldName, Exist, ErrorString))
		{
			if (!Exist)//Если поле не существует
			{
				QString AddColumn = "ALTER TABLE public." + MetaTable->Name + " \n" +
					"ADD COLUMN \"" + FieldName + "\" " + ISMetaData::Instance().GetTypeDB(MetaField->Type);

				if (MetaField->Size) //Если указан размер поля
				{
					AddColumn += QString("(%1)").arg(MetaField->Size);
				}

				if (MetaField->DefaultValue.isValid()) //Если указано значение по умолчанию
				{
					AddColumn += " DEFAULT " + MetaField->DefaultValue.toString(); //Указание NULL без кавычек
				}

				if (MetaField->NotNull)
				{
					AddColumn += " NOT NULL";
				}

				ISQuery qAddColumn;
				qAddColumn.SetShowLongQuery(false);
				Result = qAddColumn.Execute(AddColumn);
				if (Result)
				{
					ISDEBUG_L(QString("Add field: %1_%2").arg(MetaTable->Alias).arg(MetaField->Name));
				}
				else
				{
					ErrorString = qAddColumn.GetErrorString();
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_DeleteFields(PMetaTable *MetaTable, QString &ErrorString)
{
	ISQuery qSelectColumns(QS_OLD_COLUMNS);
	qSelectColumns.SetShowLongQuery(false);
	qSelectColumns.BindValue(":TableName", MetaTable->Name.toLower());
	bool Result = qSelectColumns.Execute();
	if (Result)
	{
		while (qSelectColumns.Next())
		{
			QString FieldFullName = qSelectColumns.ReadColumn("column_full_name").toString();
			QString FieldName = qSelectColumns.ReadColumn("column_name").toString();

			//Если поле существует в базе, но не существует в мета-данных - предлагаем удалить
			//или если это поле есть в базе и оно имеет мета-запрос (т.е. оно виртуальное) - предлагаем удалить
			//Во второму случае такое условие можно объяснить так: было поле в базе, потом его сделали виртуальным - соответственно оно в базе на ни к чему - удаляем
			if (!MetaTable->ContainsField(FieldName) || (MetaTable->ContainsField(FieldName) && !MetaTable->GetField(FieldName)->QueryText.isEmpty()))
			{
				if (ISConsole::Question(QString("The field \"%1\" in the table \"%2\" is out of date, delete?").arg(FieldFullName).arg(MetaTable->Name))) //Пользователь согласился удалить поле - удаляем
				{
					ISQuery qDropColumn;
					qDropColumn.SetShowLongQuery(false);
					Result = qDropColumn.Execute("ALTER TABLE public." + MetaTable->Name + " DROP COLUMN " + FieldFullName);
					if (!Result)
					{
						ErrorString = qDropColumn.GetErrorString();
						break;
					}
				}
				else //Не согласен удалять поле - пропускаем и идём дальше
				{
					ISDEBUG_L("Skip deletion field " + FieldFullName);
				}
			}
		}
	}
	else
	{
		ErrorString = qSelectColumns.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Helper_ExistField(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectColumn(QS_COLUMN);
	qSelectColumn.SetShowLongQuery(false);
	qSelectColumn.BindValue(":TableName", MetaTable->Name.toLower());
	qSelectColumn.BindValue(":ColumnName", ColumnName);
	bool Result = qSelectColumn.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectColumn.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectColumn.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Helper_CommentTable(PMetaTable *MetaTable, QString &ErrorString)
{
	QString CommentText = ISSystem::VariantMapToJsonString(
	{
		{ "Name", MetaTable->Name },
		{ "UID", MetaTable->UID },
		{ "Alias", MetaTable->Alias },
		{ "LocalName", MetaTable->LocalName },
		{ "LocalListName", MetaTable->LocalListName },
		{ "TitleName", MetaTable->TitleName },
		{ "ObjectForm", MetaTable->ObjectForm },
		{ "ShowOnly", MetaTable->ShowOnly },
		{ "IsSystem", MetaTable->IsSystem }
	});

	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	bool Result = qComment.Execute(QString("COMMENT ON TABLE public.%1 IS '%2'").arg(MetaTable->Name.toLower()).arg(CommentText));
	if (!Result)
	{
		ErrorString = qComment.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Helper_CommentField(PMetaTable *MetaTable, PMetaField *MetaField, QString &ErrorString)
{
	if (!MetaField->QueryText.isEmpty()) //Если поле является виртуальным - выходим с положительным результатом
	{
		return true;
	}

	QString CommentText = ISSystem::VariantMapToJsonString(
	{
		{ "UID", MetaField->UID },
		{ "Name", MetaField->Name },
		{ "Type", ISMetaData::Instance().GetTypeDB(MetaField->Type) },
		{ "Size", MetaField->Size },
		{ "LabelName", MetaField->LabelName },
		{ "LocalListName", MetaField->LocalListName },
		{ "NotNull", MetaField->NotNull },
		{ "ReadOnly", MetaField->ReadOnly },
		{ "HideFromObject", MetaField->HideFromObject },
		{ "HideFromList", MetaField->HideFromList },
		{ "Hint", MetaField->Hint },
		{ "IsSystem", MetaField->IsSystem }
	});

	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	bool Result = qComment.Execute(QString("COMMENT ON COLUMN public.%1.%2 IS '%3'").arg(MetaTable->Name.toLower()).arg(MetaTable->Alias + '_' + MetaField->Name.toLower()).arg(CommentText));
	if (!Result)
	{
		ErrorString = qComment.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Helper_CommentFunction(PMetaFunction *MetaFunction, QString &ErrorString)
{
	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	bool Result = qComment.Execute(QString("COMMENT ON FUNCTION " + MetaFunction->Name + " IS '" + MetaFunction->Comment + "'"));
	if (!Result)
	{
		ErrorString = qComment.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
