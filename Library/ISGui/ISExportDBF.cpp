#include "ISExportDBF.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISExportDBF::ISExportDBF(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent)
	: ISExportWorker(meta_table, tcp_model, parent),
	ConnectionName(ISSystem::GenerateUuid())
{
	
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
	QString CreateTableSql = "CREATE TABLE " + MetaTable->Name + " (";
	for (const unsigned int &Index : Fields) //Обход выбранных для экспорта полей
	{
		CreateTableSql += TcpModel->GetField(Index).Name + " VARCHAR, ";
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
	
	for (int Row = 0; Row < TcpModel->rowCount(); ++Row) //Обход строк
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

		ISModelRecord Record = TcpModel->GetRecord(Row); //Текущая строка
		ISStringToVariantMap Bind;
		QString InsertFields = "INSERT INTO [" + MetaTable->Name + "](";
		QString ValuesField = "VALUES(";

		for (const unsigned int &Index : Fields) //Обход колонок
		{
			QString FieldName = TcpModel->GetField(Index).Name;
			InsertFields += "[" + FieldName + "], ";
			ValuesField += ':' + FieldName + ", ";
			Bind[':' + FieldName] = Record.Values[Index];
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

		for (const auto &MapItem : Bind)
		{
			SqlQuery.bindValue(MapItem.first, MapItem.second);
		}

		if (!SqlQuery.exec())
		{
			ErrorString = SqlQuery.lastError().text();
			return false;
		}

		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(TcpModel->rowCount()));
	}

	return true;
}
//-----------------------------------------------------------------------------
