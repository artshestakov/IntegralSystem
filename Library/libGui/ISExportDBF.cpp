#include "StdAfx.h"
#include "ISExportDBF.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISExportDBF::ISExportDBF(QObject *parent) : ISExportWorker(parent)
{
	ConnectionName = ISSystem::GenerateUuid();
}
//-----------------------------------------------------------------------------
ISExportDBF::~ISExportDBF()
{
	QSqlDatabase::database(ConnectionName).close();
	QSqlDatabase::removeDatabase(ConnectionName);
}
//-----------------------------------------------------------------------------
bool ISExportDBF::Prepare()
{
	QSqlDatabase Database = QSqlDatabase::addDatabase("QODBC", ConnectionName);
	Database.setDatabaseName("Driver={Microsoft dBase Driver (*.dbf)};SourceType=DBF;");
	if (!Database.open())
	{
		ErrorString = Database.lastError().text();
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISExportDBF::Export()
{
	QString CreateTableSql = "CREATE TABLE " + TableName + " (";
	
	for (int i = 0; i < Fields.count(); ++i) //Обход выбранных для экспорта полей
	{
		QString FieldName = Fields.at(i);
		if (Fields.contains(FieldName))
		{
			CreateTableSql += FieldName + " VARCHAR, ";
		}
	}

	ISSystem::RemoveLastSymbolFromString(CreateTableSql, 2);
	CreateTableSql += ")";

	QSqlQuery SqlQuery(QSqlDatabase::database(ConnectionName));
	if (!SqlQuery.prepare(CreateTableSql))
	{
		ErrorString = SqlQuery.lastError().text();
		return false;
	}

	if (!SqlQuery.exec())
	{
		ErrorString = SqlQuery.lastError().text();
		return false;
	}
	
	for (int Row = 0; Row < Model->rowCount(); Row++) //Обход строк
	{
		if (Canceled) //Если была нажата кнопка "Остановить"
		{
			if (ISMessageBox::ShowQuestion(nullptr, LANG("Export.Process.Cancel"), LANG("Export.Process.Cancel.DetailedText")))
			{
				return true;
			}
			else
			{
				Canceled = false;
			}
		}

		if (SelectedRows.count()) //Если есть выделенные строки
		{
			if (!SelectedRows.contains(Row))
			{
				continue;
			}
		}

		QSqlRecord SqlRecord = Model->GetRecord(Row); //Текущая строка
		QStringMap Bind;
		QString InsertFields = "INSERT INTO " + TableName + "(";
		QString ValuesField = "VALUES(";

		for (int Column = 0; Column < Fields.count(); Column++) //Обход колонок
		{
			QString FieldName = Fields.at(Column);
			QString FieldValue = SqlRecord.value(FieldName).toString();
			
			InsertFields += FieldName + ", ";
			ValuesField += ":" + FieldName + ", ";

			Bind.insert(":" + FieldName, FieldValue);
		}

		ISSystem::RemoveLastSymbolFromString(InsertFields, 2);
		InsertFields += ")";

		ISSystem::RemoveLastSymbolFromString(ValuesField, 2);
		ValuesField += ")";

		if (!SqlQuery.prepare(InsertFields + SYMBOL_SPACE + ValuesField))
		{
			ErrorString = SqlQuery.lastError().text();
			return false;
		}

		for (const auto &Item : Bind.toStdMap())
		{
			SqlQuery.bindValue(Item.first, Item.second);
		}

		if (!SqlQuery.exec())
		{
			ErrorString = SqlQuery.lastError().text();
			return false;
		}

		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(Model->rowCount()) + "...");
	}

	return true;
}
//-----------------------------------------------------------------------------
