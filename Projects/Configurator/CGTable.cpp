#include "CGTable.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "CGTemplateField.h"
#include "CGHelper.h"
#include "CGSequence.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISConsole.h"
#include "ISLogger.h"
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
bool CGTable::CreateTable(PMetaTable *MetaTable, QString &ErrorString)
{
	QString TableName = MetaTable->Name.toLower(), TableAlias = MetaTable->Alias.toLower();

	bool Exist = true;
	bool Result = CGSequence::CheckExistSequence(TableName, Exist, ErrorString);
	if (Result)
	{
		if (!Exist)
		{
			Result = CGSequence::CreateSequence(TableName, ErrorString);
		}
	}

	if (!Result)
	{
		return Result;
	}

	QString SqlText = "CREATE TABLE public." + MetaTable->Name.toLower() + " \n( \n";
	SqlText += CGTemplateField::GetSqlTextForTemplateSystemFields(TableName, TableAlias);

	//������������ ������� �� �������� �������
	for (PMetaField *MetaField : MetaTable->Fields) //����� ����� �������
	{
		if (!MetaField->QueryText.isEmpty())
		{
			continue;
		}

		ISNamespace::FieldType FieldType = MetaField->Type; //��� ����
		QString FieldName = MetaField->Name; //��� ����
		int FieldSize = MetaField->Size; //������ ����
		QString FieldDefalutValue = MetaField->DefaultValue.toString(); //�������� �� ��������� ��� ����
		bool FieldNotNull = MetaField->NotNull; //������ ������������� ���������� ����

		SqlText += "\t" + TableAlias + '_' + FieldName.toLower() + SYMBOL_SPACE + ISMetaData::GetInstanse().GetTypeDB(FieldType);

		if (FieldSize > 0) //���� ������ ������ ����
		{
			SqlText += '(' + QString::number(FieldSize) + ')';
		}

		if (FieldDefalutValue.isEmpty()) //���� �������� �� ��������� �� �������
		{
			SqlText += " DEFAULT NULL";
		}
		else //�������� �� ��������� �������
		{
			SqlText += " DEFAULT " + (FieldType == ISNamespace::FT_UID ? QString("'%1'").arg(FieldDefalutValue) : FieldDefalutValue);
		}

		if (FieldNotNull) //���� ���� ����������� ��� ����������
		{
			SqlText += " NOT NULL";
		}

		SqlText += ",\n";
	}

	//���������� ������������ ������� �� �������� �������
	SqlText.chop(2);
	SqlText += "\n);";

	//���������� �������
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
bool CGTable::UpdateTable(PMetaTable *MetaTable, QString &ErrorString)
{
	bool Result = AlterExistFields(MetaTable, ErrorString);
	if (Result)
	{
		Result = CreateNewFields(MetaTable, ErrorString);
	}

	if (Result)
	{
		Result = DeleteOldFields(MetaTable, ErrorString);
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGTable::CheckExistTable(PMetaTable *MetaTable, bool &Exist, QString &ErrorString)
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
bool CGTable::AlterExistFields(PMetaTable *MetaTable, QString &ErrorString)
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

			if (!ISMetaData::GetInstanse().CheckExitField(MetaTable, ColumnName))
			{
				continue;
			}

			PMetaField *MetaField = ISMetaData::GetInstanse().GetMetaField(MetaTable, ColumnName);
			QString MetaType = ISMetaData::GetInstanse().GetTypeDB(MetaField->Type);
			QString MetaDefaultValue = MetaField->DefaultValue.toString();
			bool MetaNotNull = MetaField->NotNull;
			int MetaSize = MetaField->Size;

			//�������� ������������ �����
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

			//���� ���� �������� ID - �� �� ���������� ������
			if (ColumnName == "id")
			{
				continue;
			}

			//�������� ������������ �������� �� ���������
			if (ColumnDefaultValue != MetaDefaultValue)
			{
				QString QueryText;
				if (MetaDefaultValue.isEmpty()) //������� �������� �� ���������
				{
					QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN " + ColumnNameFull + " DROP DEFAULT";
				}
				else //���� �������� �� ��������� �������
				{
					if (MetaDefaultValue.toLower() == "null") //���� �������� �� ��������� ������� �������
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

			//�������� ������������ ����� ������������� ����������
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
bool CGTable::CreateNewFields(PMetaTable *MetaTable, QString &ErrorString)
{
	bool Result = true, Exist = true;
	for (PMetaField *MetaField : MetaTable->Fields) //����� �����
	{
		if (!MetaField->QueryText.isEmpty())
		{
			continue;
		}

		QString FieldName = MetaTable->Alias + '_' + MetaField->Name.toLower();
		if (CGHelper::CheckExistColumn(MetaTable, FieldName, Exist, ErrorString))
		{
			if (!Exist)//���� ���� �� ����������
			{
				QString AddColumn = "ALTER TABLE public." + MetaTable->Name + " \n" +
					"ADD COLUMN \"" + FieldName + "\"" + SYMBOL_SPACE + ISMetaData::GetInstanse().GetTypeDB(MetaField->Type);

				if (MetaField->Size) //���� ������ ������ ����
				{
					AddColumn += QString("(%1)").arg(MetaField->Size);
				}

				if (!MetaField->DefaultValue.toString().isEmpty()) //���� ������� �������� �� ���������
				{
					if (MetaField->Type == ISNamespace::FT_Int ||
						MetaField->Type == ISNamespace::FT_Date ||
						MetaField->Type == ISNamespace::FT_Time ||
						MetaField->Type == ISNamespace::FT_DateTime)
					{
						AddColumn += " DEFAULT " + MetaField->DefaultValue.toString(); //�������� NULL ��� �������
					}
					else
					{
						AddColumn += " DEFAULT '" + MetaField->DefaultValue.toString() + '\''; //�������� NULL � �������
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
bool CGTable::DeleteOldFields(PMetaTable *MetaTable, QString &ErrorString)
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
			if (!MetaTable->ContainsField(FieldName)) //���� ���� ���������� � ����, �� �� ���������� � ����-������ - ���������� �������
			{
				if (ISConsole::Question(QString("The field \"%1\" in the table \"%2\" is out of date, delete?").arg(FieldFullName).arg(MetaTable->Name))) //������������ ���������� ������� ���� - �������
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
				else //�� �������� ������� ���� - ���������� � ��� ������
				{
					ISLOGGER_L("Skip deletion field " + FieldFullName);
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
