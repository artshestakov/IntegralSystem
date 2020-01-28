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

	//�������� ������������� ������������������, ���� ������������������ �� ���������� - ���������� � ��������
	if (!CGSequence::CheckExistSequence(TableName))
	{
		CGSequence::CreateSequence(TableName);
	}

	QString SqlText = "CREATE TABLE public." + MetaTable->GetName().toLower() + "(\n";
	SqlText += CGTemplateField::GetSqlTextForTemplateSystemFields(TableName, TableAlias);

	//������������ ������� �� �������� �������
	int CountFields = MetaTable->GetFields().count();
	for (int i = 0; i < CountFields; ++i) //����� ����� �������
	{
		PMetaClassField *MetaField = MetaTable->GetFields().at(i);
		ISNamespace::FieldType FieldType = MetaField->GetType(); //��� ����
		
		if (MetaField->GetQueryText().length())
		{
			continue;
		}

		QString FieldName = MetaField->GetName(); //��� ����
		int FieldSize = MetaField->GetSize(); //������ ����
		QString FieldDefalutValue = MetaField->GetDefaultValue().toString(); //�������� �� ��������� ��� ����
		bool FieldNotNull = MetaField->GetNotNull(); //������ ������������� ���������� ����

		SqlText += TableAlias + "_" + FieldName.toLower();
		SqlText += SYMBOL_SPACE + ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(FieldType);

		if (FieldSize) //���� ������ ������ ����
		{
			SqlText += "(" + QString::number(FieldSize) + ")";
		}

		if (FieldDefalutValue.length()) //���� ������� �������� �� ���������
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

			//�������� ������������ �����
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

			//�������� ������������ �������� �� ���������
			if (ColumnDefaultValue != MetaDefaultValue)
			{
				QString QueryText;

				if (MetaDefaultValue.length()) //���� �������� �� ��������� �������
				{
					if (MetaDefaultValue.toLower() == "null") //���� �������� �� ��������� ������� �������
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
				else //������� �������� �� ���������
				{
					QueryText = "ALTER TABLE public." + MetaTable->GetName() + " ALTER COLUMN " + ColumnNameFull + " DROP DEFAULT";
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
	for (int i = 0; i < MetaTable->GetFields().count(); ++i) //����� �����
	{
		PMetaClassField *MetaField = MetaTable->GetFields().at(i);
		if (MetaField->GetQueryText().length())
		{
			continue;
		}

		QString FieldName = MetaTable->GetAlias() + "_" + MetaField->GetName().toLower();

		if (!CGHelper::CheckExistColumn(MetaTable, FieldName)) //���� ���� �� ����������
		{
			QString AddColumn = "ALTER TABLE public." + MetaTable->GetName() + " \n" +
				"ADD COLUMN \"" + FieldName + "\" " + ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(MetaField->GetType());

			if (MetaField->GetSize()) //���� ������ ������ ����
			{
				AddColumn += "(" + QString::number(MetaField->GetSize()) + ")";
			}

			if (!MetaField->GetDefaultValue().toString().isEmpty()) //���� ������� �������� �� ���������
			{
				if (MetaField->GetType() == ISNamespace::FT_Int ||
					MetaField->GetType() == ISNamespace::FT_Date ||
					MetaField->GetType() == ISNamespace::FT_Time ||
					MetaField->GetType() == ISNamespace::FT_DateTime)
				{
					AddColumn += " DEFAULT " + MetaField->GetDefaultValue().toString(); //�������� NULL ��� �������
				}
				else
				{
					AddColumn += " DEFAULT '" + MetaField->GetDefaultValue().toString() + "'"; //�������� NULL � �������
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
