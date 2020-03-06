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
void CGTable::CreateTable(PMetaClassTable *MetaTable, QString &ErrorString)
{
	QString TableName = MetaTable->Name.toLower();
	QString TableAlias = MetaTable->Alias;

	//�������� ������������� ������������������, ���� ������������������ �� ���������� - ���������� � ��������
	if (!CGSequence::CheckExistSequence(TableName))
	{
		CGSequence::CreateSequence(TableName);
	}

	QString SqlText = "CREATE TABLE public." + MetaTable->Name.toLower() + "(\n";
	SqlText += CGTemplateField::GetSqlTextForTemplateSystemFields(TableName, TableAlias);

	//������������ ������� �� �������� �������
	int CountFields = MetaTable->Fields.size();
	for (int i = 0; i < CountFields; ++i) //����� ����� �������
	{
		PMetaClassField *MetaField = MetaTable->Fields[i];
		ISNamespace::FieldType FieldType = MetaField->Type; //��� ����
		
		if (MetaField->QueryText.length())
		{
			continue;
		}

		QString FieldName = MetaField->Name; //��� ����
		int FieldSize = MetaField->Size; //������ ����
		QString FieldDefalutValue = MetaField->DefaultValue.toString(); //�������� �� ��������� ��� ����
		bool FieldNotNull = MetaField->NotNull; //������ ������������� ���������� ����

		SqlText += TableAlias + '_' + FieldName.toLower();
		SqlText += SYMBOL_SPACE + ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(FieldType);

		if (FieldSize) //���� ������ ������ ����
		{
			SqlText += '(' + QString::number(FieldSize) + ')';
		}

		if (FieldDefalutValue.length()) //���� ������� �������� �� ���������
		{
			if (FieldType == ISNamespace::FT_UID)
			{
				SqlText += " DEFAULT '" + FieldDefalutValue + '\'';
			}
			else
			{
				SqlText += " DEFAULT " + FieldDefalutValue;
			}
		}
		else //����� �������� ��� ��������� NULL
		{
			SqlText += " DEFAULT NULL";
		}

		if (FieldNotNull) //���� ���� ����������� ��� ����������
		{
			SqlText += " NOT NULL";
		}

		SqlText += ",\n";
	}

	//���������� ������������ ������� �� �������� �������
	ISSystem::RemoveLastSymbolFromString(SqlText, 2);
	SqlText += "\n);";

	//���������� �������
	ISQuery qCreateTable;
	qCreateTable.SetShowLongQuery(false);
	bool Created = qCreateTable.Execute(SqlText);

	if (Created)
	{
		for (int i = 0; i < MetaTable->AllFields.size(); ++i)
		{
			PMetaClassField *MetaField = MetaTable->AllFields[i];
			if (MetaField->QueryText.length())
			{
				continue;
			}

			CGHelper::CommentField(MetaTable->Name.toLower(), MetaTable->Alias + '_' + MetaField->Name.toLower(), MetaField->LocalListName);
		}
	}
	else
	{
		ErrorString = qCreateTable.GetErrorString();
	}
}
//-----------------------------------------------------------------------------
void CGTable::UpdateTable(PMetaClassTable *MetaTable)
{
	CGHelper::CommentTable(MetaTable->Name.toLower(), MetaTable->LocalListName);
	AlterExistFields(MetaTable);
	CreateNewFields(MetaTable);
}
//-----------------------------------------------------------------------------
bool CGTable::CheckExistTable(PMetaClassTable *MetaTable)
{
	ISQuery qSelectTable(QS_TABLE);
	qSelectTable.SetShowLongQuery(false);
	qSelectTable.BindValue(":TableName", MetaTable->Name.toLower());
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
	qSelectColumns.BindValue(":TableName", MetaTable->Name.toLower());
	if (qSelectColumns.Execute())
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
			QString MetaType = ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->Type);
			QString MetaDefaultValue = MetaField->DefaultValue.toString();
			bool MetaNotNull = MetaField->NotNull;
			int MetaSize = MetaField->Size;

			CGHelper::CommentField(MetaTable->Name.toLower(), MetaTable->Alias + '_' + MetaField->Name.toLower(), MetaField->LocalListName);

			if (ColumnName == "id")
			{
				continue;
			}

			//�������� ������������ �����
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

			//�������� ������������ �������� �� ���������
			if (ColumnDefaultValue != MetaDefaultValue)
			{
				QString QueryText;

				if (MetaDefaultValue.length()) //���� �������� �� ��������� �������
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
				else //������� �������� �� ���������
				{
					QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN " + ColumnNameFull + " DROP DEFAULT";
				}

				ISQuery qAlterColumn;
				qAlterColumn.SetShowLongQuery(false);
				qAlterColumn.Execute(QueryText);
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
				qAlterNotNull.Execute(QueryText);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CGTable::CreateNewFields(PMetaClassTable *MetaTable)
{
	for (int i = 0; i < MetaTable->Fields.size(); ++i) //����� �����
	{
		PMetaClassField *MetaField = MetaTable->Fields[i];
		if (!MetaField->QueryText.isEmpty())
		{
			continue;
		}

		QString FieldName = MetaTable->Alias + '_' + MetaField->Name.toLower();

		if (!CGHelper::CheckExistColumn(MetaTable, FieldName)) //���� ���� �� ����������
		{
			QString AddColumn = "ALTER TABLE public." + MetaTable->Name + " \n" +
				"ADD COLUMN \"" + FieldName + "\"" + SYMBOL_SPACE + ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->Type);

			if (MetaField->Size) //���� ������ ������ ����
			{
				AddColumn += '(' + QString::number(MetaField->Size) + ')';
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
			if (qAddColumn.Execute(AddColumn))
			{
				CGHelper::CommentField(MetaTable->Name.toLower(), MetaTable->Alias + '_' + MetaField->Name.toLower(), MetaField->LocalListName);
			}
		}
	}
}
//-----------------------------------------------------------------------------
