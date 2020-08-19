#include "ISExportDBF.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
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
	
	for (const QString &FieldName : Fields) //Обход выбранных для экспорта полей
	{
		if (ISAlgorithm::VectorContains(Fields, FieldName))
		{
			CreateTableSql += FieldName + " VARCHAR, ";
		}
	}

	CreateTableSql.chop(2);
	CreateTableSql += ')';

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
	
	for (int Row = 0; Row < Model->rowCount(); ++Row) //Обход строк
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

		if (!SelectedRows.empty()) //Если есть выделенные строки
		{
			if (!ISAlgorithm::VectorContains(SelectedRows, Row))
			{
				continue;
			}
		}

		QSqlRecord SqlRecord = Model->GetRecord(Row); //Текущая строка
		ISStringMap Bind;
		QString InsertFields = "INSERT INTO " + TableName + '(';
		QString ValuesField = "VALUES(";

		for (const QString &FieldName : Fields) //Обход колонок
		{
			InsertFields += FieldName + ", ";
			ValuesField += ':' + FieldName + ", ";
			Bind.emplace(':' + FieldName, SqlRecord.value(FieldName).toString());
		}

		InsertFields.chop(2);
		InsertFields += ')';

		ValuesField.chop(2);
		ValuesField += ')';

		if (!SqlQuery.prepare(InsertFields + SYMBOL_SPACE + ValuesField))
		{
			ErrorString = SqlQuery.lastError().text();
			return false;
		}

		for (const auto &Item : Bind)
		{
			SqlQuery.bindValue(Item.first, Item.second);
		}

		if (!SqlQuery.exec())
		{
			ErrorString = SqlQuery.lastError().text();
			return false;
		}

		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(Model->rowCount()));
	}

	return true;
}
//-----------------------------------------------------------------------------
