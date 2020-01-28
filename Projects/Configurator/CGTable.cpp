#include "StdAfx.h"
#include "CGTable.h"
#include "ISDefines.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISAssert.h"
#include "CGTemplateField.h"
#include "CGHelper.h"
#include "CGSequence.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QS_TABLE = PREPARE_QUERY("SELECT COUNT(*) FROM pg_tables WHERE schemaname = current_schema() AND tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QS_COLUMNS_COUNT = PREPARE_QUERY("SELECT COUNT(*) "
								  "FROM information_schema.columns "
								  "WHERE table_catalog = current_database() "
								  "AND table_schema = current_schema() "
								  "AND table_name = :TableName");
//-----------------------------------------------------------------------------
static QString QS_COLUMNS = PREPARE_QUERY("SELECT column_name, column_default, is_nullable::BOOLEAN, data_type, character_maximum_length "
										  "FROM information_schema.columns "
										  "WHERE table_catalog = current_database() "
										  "AND table_schema = current_schema() "
										  "AND table_name = :TableName "
										  "ORDER BY ordinal_position");
//-----------------------------------------------------------------------------
CGTable::CGTable() : QObject()
{

}
//-----------------------------------------------------------------------------
CGTable::~CGTable()
{

}
//-----------------------------------------------------------------------------
void CGTable::CreateTable(PMetaClassTable *MetaTable, QString &ErrorString)
{
	QString TableName = MetaTable->GetName().toLower();
	QString TableAlias = MetaTable->GetAlias();

	//Проверка существования последовательности, если последовательность не существует - произвести её создание
	if (!CGSequence::CheckExistSequence(TableName))
	{
		CGSequence::CreateSequence(TableName);
	}

	QString SqlText = "CREATE TABLE public." + MetaTable->GetName().toLower() + "(\n";
	SqlText += CGTemplateField::GetSqlTextForTemplateSystemFields(TableName, TableAlias);

	//Формирование запроса на создание таблицы
	int CountFields = MetaTable->GetFields().count();
	for (int i = 0; i < CountFields; ++i) //Обход полей таблицы
	{
		PMetaClassField *MetaField = MetaTable->GetFields().at(i);
		ISNamespace::FieldType FieldType = MetaField->GetType(); //Тип поля
		
		if (MetaField->GetQueryText().length())
		{
			continue;
		}

		QString FieldName = MetaField->GetName(); //Имя поля
		int FieldSize = MetaField->GetSize(); //Размер поля
		QString FieldDefalutValue = MetaField->GetDefaultValue().toString(); //Значение по умолчанию для поля
		bool FieldNotNull = MetaField->GetNotNull(); //Статус обязательного заполнения поля

		SqlText += TableAlias + "_" + FieldName.toLower();
		SqlText += SYMBOL_SPACE + ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(FieldType);

		if (FieldSize) //Если указан размер поля
		{
			SqlText += "(" + QString::number(FieldSize) + ")";
		}

		if (FieldDefalutValue.length()) //Если указано значение по умолчанию
		{
			if (FieldType == ISNamespace::FT_UID)
			{
				SqlText += " DEFAULT '" + FieldDefalutValue + "'";
			}
			else
			{
				SqlText += " DEFAULT " + FieldDefalutValue;
			}
		}
		else //Иначе значение пол умолчанию NULL
		{
			SqlText += " DEFAULT NULL";
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
	bool Created = qCreateTable.Execute(SqlText);

	if (Created)
	{
		for (int i = 0; i < MetaTable->GetAllFields().count(); ++i)
		{
			PMetaClassField *MetaField = MetaTable->GetAllFields().at(i);
			if (MetaField->GetQueryText().length())
			{
				continue;
			}

			CGHelper::CommentField(MetaTable->GetName().toLower(), MetaTable->GetAlias() + "_" + MetaField->GetName().toLower(), MetaField->GetLocalListName());
		}
	}
	else
	{
		ErrorString = qCreateTable.GetErrorText();
	}
}
//-----------------------------------------------------------------------------
void CGTable::UpdateTable(PMetaClassTable *MetaTable)
{
	CGHelper::CommentTable(MetaTable->GetName().toLower(), MetaTable->GetLocalListName());
	AlterExistFields(MetaTable);
	CreateNewFields(MetaTable);
}
//-----------------------------------------------------------------------------
bool CGTable::CheckExistTable(PMetaClassTable *MetaTable)
{
	ISQuery qSelectTable(QS_TABLE);
	qSelectTable.SetShowLongQuery(false);
	qSelectTable.BindValue(":TableName", MetaTable->GetName().toLower());
	if (qSelectTable.ExecuteFirst())
	{
		if (qSelectTable.ReadColumn("count").toInt() == 1)
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
void CGTable::AlterExistFields(PMetaClassTable *MetaTable)
{
	ISQuery qSelectColumns(QS_COLUMNS);
	qSelectColumns.SetShowLongQuery(false);
	qSelectColumns.BindValue(":TableName", MetaTable->GetName().toLower());
	if (qSelectColumns.Execute())
	{
		while (qSelectColumns.Next())
		{
			QString ColumnNameFull = qSelectColumns.ReadColumn("column_name").toString();
			QString ColumnName = qSelectColumns.ReadColumn("column_name").toString().split("_").at(1);
			QString ColumnDefaultValue = qSelectColumns.ReadColumn("column_default").toString();
			bool ColumnNotNull = !qSelectColumns.ReadColumn("is_nullable").toBool();
			QString ColumnType = qSelectColumns.ReadColumn("data_type").toString().toUpper();
			int ColumnSize = qSelectColumns.ReadColumn("character_maximum_length").toInt();

			if (!ISMetaData::GetInstanse().CheckExitField(MetaTable, ColumnName))
			{
				continue;
			}

			PMetaClassField *MetaField = ISMetaData::GetInstanse().GetMetaField(MetaTable, ColumnName);
			QString MetaType = ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->GetType());
			QString MetaDefaultValue = MetaField->GetDefaultValue().toString();
			bool MetaNotNull = MetaField->GetNotNull();
			int MetaSize = MetaField->GetSize();

			CGHelper::CommentField(MetaTable->GetName().toLower(), MetaTable->GetAlias() + "_" + MetaField->GetName().toLower(), MetaField->GetLocalListName());

			if (ColumnName == "id")
			{
				continue;
			}

			//Проверка соответствия типов
			if (ColumnType != MetaType)
			{
				QString QueryText;
				QueryText += "ALTER TABLE public." + MetaTable->GetName().toLower() + " \n";
				QueryText += "ALTER COLUMN " + MetaTable->GetAlias() + "_" + MetaField->GetName().toLower() + " TYPE " + MetaType;// +" \n";

				if (MetaSize)
				{
					QueryText += "(" + QString::number(MetaSize) + ") \n";
				}
				else
				{
					QueryText += " \n";
				}

				QueryText += "USING " + MetaTable->GetAlias() + "_" + MetaField->GetName().toLower() + "::" + MetaType;

				ISQuery qAlterType;
				qAlterType.SetShowLongQuery(false);
				qAlterType.Execute(QueryText);
			}

			//Проверка соответствия значения по умолчанию
			if (ColumnDefaultValue != MetaDefaultValue)
			{
				QString QueryText;

				if (MetaDefaultValue.length()) //Если значение по умолчанию указано
				{
					if (MetaDefaultValue.toLower() == "null") //Если значение по умолчанию указано НУЛЕВОЕ
					{
						QueryText = "ALTER TABLE public." + MetaTable->GetName() + " ALTER COLUMN " + ColumnNameFull + " SET DEFAULT NULL";
					}
					else
					{
						if (MetaField->GetType() == ISNamespace::FT_Int ||
							MetaField->GetType() == ISNamespace::FT_DateTime ||
							MetaField->GetType() == ISNamespace::FT_Date ||
							MetaField->GetType() == ISNamespace::FT_Time ||
							MetaField->GetType() == ISNamespace::FT_UID)
						{
							QueryText = "ALTER TABLE public." + MetaTable->GetName() + " ALTER COLUMN " + ColumnNameFull + " SET DEFAULT " + MetaDefaultValue;
						}
						else
						{
							QueryText = "ALTER TABLE public." + MetaTable->GetName() + " ALTER COLUMN " + ColumnNameFull + " SET DEFAULT '" + MetaDefaultValue + "'";
						}
					}
				}
				else //Удалить значение по умолчанию
				{
					QueryText = "ALTER TABLE public." + MetaTable->GetName() + " ALTER COLUMN " + ColumnNameFull + " DROP DEFAULT";
				}

				ISQuery qAlterColumn;
				qAlterColumn.SetShowLongQuery(false);
				qAlterColumn.Execute(QueryText);
			}

			//Проверка соответствия флага обязательного заполнения
			if (ColumnNotNull != MetaNotNull)
			{
				QString QueryText;
				if (!ColumnNotNull && MetaNotNull)
				{
					QueryText = "ALTER TABLE public." + MetaTable->GetName() + " ALTER COLUMN " + MetaTable->GetAlias() + "_" + MetaField->GetName() + " SET NOT NULL";
				}
				else if (ColumnNotNull && !MetaNotNull)
				{
					QueryText = "ALTER TABLE public." + MetaTable->GetName() + " ALTER COLUMN " + MetaTable->GetAlias() + "_" + MetaField->GetName() + " DROP NOT NULL";
				}

				ISQuery qAlterNotNull;
				qAlterNotNull.SetShowLongQuery(false);
				qAlterNotNull.Execute(QueryText);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CGTable::CreateNewFields(PMetaClassTable *MetaTable)
{
	for (int i = 0; i < MetaTable->GetFields().count(); ++i) //Обход полей
	{
		PMetaClassField *MetaField = MetaTable->GetFields().at(i);
		if (MetaField->GetQueryText().length())
		{
			continue;
		}

		QString FieldName = MetaTable->GetAlias() + "_" + MetaField->GetName().toLower();

		if (!CGHelper::CheckExistColumn(MetaTable, FieldName)) //Если поле не существует
		{
			QString AddColumn = "ALTER TABLE public." + MetaTable->GetName() + " \n" +
				"ADD COLUMN \"" + FieldName + "\" " + ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->GetType());

			if (MetaField->GetSize()) //Если указан размер поля
			{
				AddColumn += "(" + QString::number(MetaField->GetSize()) + ")";
			}

			if (!MetaField->GetDefaultValue().toString().isEmpty()) //Если указано значение по умолчанию
			{
				if (MetaField->GetType() == ISNamespace::FT_Int ||
					MetaField->GetType() == ISNamespace::FT_Date ||
					MetaField->GetType() == ISNamespace::FT_Time ||
					MetaField->GetType() == ISNamespace::FT_DateTime)
				{
					AddColumn += " DEFAULT " + MetaField->GetDefaultValue().toString(); //Указание NULL без кавычек
				}
				else
				{
					AddColumn += " DEFAULT '" + MetaField->GetDefaultValue().toString() + "'"; //Указание NULL с кавычек
				}
			}

			if (MetaField->GetNotNull())
			{
				AddColumn += " NOT NULL";
			}

			ISQuery qAddColumn;
			qAddColumn.SetShowLongQuery(false);
			if (qAddColumn.Execute(AddColumn))
			{
				CGHelper::CommentField(MetaTable->GetName().toLower(), MetaTable->GetAlias() + "_" + MetaField->GetName().toLower(), MetaField->GetLocalListName());
			}
		}
	}
}
//-----------------------------------------------------------------------------
