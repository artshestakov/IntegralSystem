#include "CGTable.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "CGTemplateField.h"
#include "CGHelper.h"
#include "CGSequence.h"
#include "ISSystem.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_TABLE = PREPARE_QUERY("SELECT COUNT(*) FROM pg_tables WHERE schemaname = current_schema() AND tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QS_COLUMNS = PREPARE_QUERY("SELECT column_name, column_default, is_nullable::BOOLEAN, data_type, character_maximum_length "
										  "FROM information_schema.columns "
										  "WHERE table_catalog = current_database() "
										  "AND table_schema = current_schema() "
										  "AND table_name = :TableName "
										  "ORDER BY ordinal_position");
//-----------------------------------------------------------------------------
bool CGTable::CreateTable(PMetaClassTable *MetaTable, QString &ErrorString)
{
	QString TableName = MetaTable->Name.toLower(), TableAlias = MetaTable->Alias.toLower();

	//Проверка существования последовательности, если последовательность не существует - произвести её создание
	bool Exist = true;
	bool Result = CGSequence::CheckExistSequence(TableName, Exist, ErrorString);
	if (Result)
	{
		if (!Exist) //Последовательность не существует
		{
			Result = CGSequence::CreateSequence(TableName);
		}
	}
	else //Ошибка проверки наличия последовательности
	{
		return Result;
	}

	QString SqlText = "CREATE TABLE public." + MetaTable->Name.toLower() + "(\n";
	SqlText += CGTemplateField::GetSqlTextForTemplateSystemFields(TableName, TableAlias);

	//Формирование запроса на создание таблицы
	for (int i = 0, CountFields = MetaTable->Fields.size(); i < CountFields; ++i) //Обход полей таблицы
	{
		PMetaClassField *MetaField = MetaTable->Fields[i];
		if (!MetaField->QueryText.isEmpty())
		{
			continue;
		}

		ISNamespace::FieldType FieldType = MetaField->Type; //Тип поля
		QString FieldName = MetaField->Name; //Имя поля
		int FieldSize = MetaField->Size; //Размер поля
		QString FieldDefalutValue = MetaField->DefaultValue.toString(); //Значение по умолчанию для поля
		bool FieldNotNull = MetaField->NotNull; //Статус обязательного заполнения поля

		SqlText += TableAlias + '_' + FieldName.toLower() + SYMBOL_SPACE + ISMetaData::GetInstanse().GetTypeDB(FieldType);

		if (FieldSize > 0) //Если указан размер поля
		{
			SqlText += '(' + QString::number(FieldSize) + ')';
		}

		if (FieldDefalutValue.isEmpty()) //Если значение по умолчанию не указано
		{
			SqlText += " DEFAULT NULL";
		}
		else //Значение по умолчанию указано
		{
			SqlText += " DEFAULT " + FieldType == ISNamespace::FT_UID ? ("'" + FieldDefalutValue + "\'") : FieldDefalutValue;			
		}

		if (FieldNotNull) //Если поле обязательно для заполнения
		{
			SqlText += " NOT NULL";
		}

		SqlText += ",\n";
	}

	//Завершение формирования запроса на создание таблицы
	ISSystem::RemoveLastSymbolFromString(SqlText, 2);
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
bool CGTable::UpdateTable(PMetaClassTable *MetaTable, QString &ErrorString)
{
	bool Result = AlterExistFields(MetaTable, ErrorString);
	if (Result)
	{
		Result = CreateNewFields(MetaTable, ErrorString);
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGTable::CheckExistTable(PMetaClassTable *MetaTable, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectTable(QS_TABLE);
	qSelectTable.SetShowLongQuery(false);
	qSelectTable.BindValue(":TableName", MetaTable->Name.toLower());
	bool Result = qSelectTable.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectTable.ReadColumn("count").toInt() > 0;
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGTable::AlterExistFields(PMetaClassTable *MetaTable, QString &ErrorString)
{
	ISQuery qSelectColumns(QS_COLUMNS);
	qSelectColumns.SetShowLongQuery(false);
	qSelectColumns.BindValue(":TableName", MetaTable->Name.toLower());
	bool Result = qSelectColumns.Execute();
	if (Result)
	{
		while (qSelectColumns.Next())
		{
			QString ColumnNameFull = qSelectColumns.ReadColumn("column_name").toString();
			QString ColumnName = qSelectColumns.ReadColumn("column_name").toString().split('_').at(1);
			QString ColumnDefaultValue = qSelectColumns.ReadColumn("column_default").toString();
			bool ColumnNotNull = !qSelectColumns.ReadColumn("is_nullable").toBool();
			QString ColumnType = qSelectColumns.ReadColumn("data_type").toString().toUpper();
			int ColumnSize = qSelectColumns.ReadColumn("character_maximum_length").toInt();

			if (!ISMetaData::GetInstanse().CheckExitField(MetaTable, ColumnName))
			{
				continue;
			}

			PMetaClassField *MetaField = ISMetaData::GetInstanse().GetMetaField(MetaTable, ColumnName);
			QString MetaType = ISMetaData::GetInstanse().GetTypeDB(MetaField->Type);
			QString MetaDefaultValue = MetaField->DefaultValue.toString();
			bool MetaNotNull = MetaField->NotNull;
			int MetaSize = MetaField->Size;

			//Проверка соответствия типов
			if (ColumnType != MetaType)
			{
				QString QueryText;
				QueryText += "ALTER TABLE public." + MetaTable->Name.toLower() + " \n";
				QueryText += "ALTER COLUMN " + MetaTable->Alias + '_' + MetaField->Name.toLower() + " TYPE " + MetaType;// +" \n";

				if (MetaSize)
				{
					QueryText += '(' + QString::number(MetaSize) + ") \n";
				}
				else
				{
					QueryText += " \n";
				}

				QueryText += "USING " + MetaTable->Alias + '_' + MetaField->Name.toLower() + "::" + MetaType;

				ISQuery qAlterType;
				qAlterType.SetShowLongQuery(false);
				qAlterType.Execute(QueryText);
			}

			//Если поле является ID - то не продолжаем дальше
			if (ColumnName == "id")
			{
				continue;
			}

			//Проверка соответствия значения по умолчанию
			if (ColumnDefaultValue != MetaDefaultValue)
			{
				QString QueryText;
				if (MetaDefaultValue.isEmpty()) //Удалить значение по умолчанию
				{
					QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN " + ColumnNameFull + " DROP DEFAULT";
				}
				else //Если значение по умолчанию указано
				{
					if (MetaDefaultValue.toLower() == "null") //Если значение по умолчанию указано НУЛЕВОЕ
					{
						QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN " + ColumnNameFull + " SET DEFAULT NULL";
					}
					else
					{
						if (MetaField->Type == ISNamespace::FT_Int ||
							MetaField->Type == ISNamespace::FT_DateTime ||
							MetaField->Type == ISNamespace::FT_Date ||
							MetaField->Type == ISNamespace::FT_Time ||
							MetaField->Type == ISNamespace::FT_UID)
						{
							QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN " + ColumnNameFull + " SET DEFAULT " + MetaDefaultValue;
						}
						else
						{
							QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN " + ColumnNameFull + " SET DEFAULT '" + MetaDefaultValue + '\'';
						}
					}
				}

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
	return Result;
}
//-----------------------------------------------------------------------------
bool CGTable::CreateNewFields(PMetaClassTable *MetaTable, QString &ErrorString)
{
	bool Result = true, Exist = true;
	for (PMetaClassField *MetaField : MetaTable->Fields) //Обход полей
	{
		if (!MetaField->QueryText.isEmpty())
		{
			continue;
		}

		QString FieldName = MetaTable->Alias + '_' + MetaField->Name.toLower();
		if (CGHelper::CheckExistColumn(MetaTable, FieldName, Exist, ErrorString))
		{
			if (!Exist)//Если поле не существует
			{
				QString AddColumn = "ALTER TABLE public." + MetaTable->Name + " \n" +
					"ADD COLUMN \"" + FieldName + "\"" + SYMBOL_SPACE + ISMetaData::GetInstanse().GetTypeDB(MetaField->Type);

				if (MetaField->Size) //Если указан размер поля
				{
					AddColumn += QString("(%1)").arg(MetaField->Size);
				}

				if (!MetaField->DefaultValue.toString().isEmpty()) //Если указано значение по умолчанию
				{
					if (MetaField->Type == ISNamespace::FT_Int ||
						MetaField->Type == ISNamespace::FT_Date ||
						MetaField->Type == ISNamespace::FT_Time ||
						MetaField->Type == ISNamespace::FT_DateTime)
					{
						AddColumn += " DEFAULT " + MetaField->DefaultValue.toString(); //Указание NULL без кавычек
					}
					else
					{
						AddColumn += " DEFAULT '" + MetaField->DefaultValue.toString() + '\''; //Указание NULL с кавычек
					}
				}

				if (MetaField->NotNull)
				{
					AddColumn += " NOT NULL";
				}

				ISQuery qAddColumn;
				qAddColumn.SetShowLongQuery(false);
				Result = qAddColumn.Execute(AddColumn);
				if (!Result)
				{
					ErrorString = ErrorString;
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
