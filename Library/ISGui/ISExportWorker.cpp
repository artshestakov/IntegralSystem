#include "ISExportWorker.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISExportWorker::ISExportWorker(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent)
	: QObject(parent),
	ErrorString(LANG("Export.Error.NoError")),
	MetaTable(meta_table),
	TcpModel(tcp_model),
	Header(false),
	Canceled(false)
{
	
}
//-----------------------------------------------------------------------------
ISExportWorker::~ISExportWorker()
{

}
//-----------------------------------------------------------------------------
void ISExportWorker::Cancel()
{
	Canceled = true;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetFields(const ISVectorUInt &fields)
{
	Fields = fields;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetHeader(bool header)
{
	Header = header;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetSelectedRows(const ISVectorInt &selected_rows)
{
	SelectedRows = selected_rows;
}
//-----------------------------------------------------------------------------
QString ISExportWorker::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
QVariant ISExportWorker::PrepareValue(ISNamespace::FieldType Type, const QVariant &Value) const
{
	QVariant Result;
	if (!Value.isNull())
	{
		if (Type == ISNamespace::FT_Bool)
		{
			Result = Value.toBool() ? LANG("Yes") : LANG("No");
		}
		else
		{
			Result = Value;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExportCSV::ISExportCSV(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent)
	: ISExportWorker(meta_table, tcp_model, parent),
	FileCSV(nullptr)
{

}
//-----------------------------------------------------------------------------
ISExportCSV::~ISExportCSV()
{

}
//-----------------------------------------------------------------------------
bool ISExportCSV::Prepare()
{
	QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Csv"), MetaTable->LocalListName);
	if (FilePath.isEmpty())
	{
		return false;
	}

	FileCSV = new QFile(FilePath, this);

	if (FileCSV->exists())
	{
		if (!FileCSV->remove())
		{
			ErrorString = FileCSV->errorString();
			return false;
		}
	}

	if (!FileCSV->open(QIODevice::WriteOnly))
	{
		ErrorString = FileCSV->errorString();
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISExportCSV::Export()
{
	if (Header) //Если в экспортируемый файл нужно добавить заголовки колонок
	{
		QString HeaderString;
		for (const unsigned int &Index : Fields) //Обход выбранных для экспорта полей
		{
			HeaderString.append(TcpModel->GetField(Index).LocalName);
			HeaderString.append(';');
		}
		HeaderString.chop(1);
		HeaderString.append("\r\n");
		FileCSV->write(HeaderString.toLocal8Bit());
	}

	for (int Row = 0; Row < TcpModel->rowCount(); ++Row) //Обход строк
	{
		if (Canceled) //Если была нажата кнопка "Остановить"
		{
			if (ISMessageBox::ShowQuestion(nullptr, LANG("Export.Process.Cancel"), LANG("Export.Process.Cancel.DetailedText")))
			{
				FileCSV->close();
				QFile::remove(FileCSV->fileName());
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
		QString RowString;
		for (const unsigned int &Index : Fields) //Обход колонок
		{
			QVariant Value = Record.Values[Index];
			Value = PrepareValue(TcpModel->GetField(Index).Type, Value);
			RowString.append(Value.toString());
			RowString.append(';');
		}
		RowString.chop(1);
		RowString.append("\r\n");
		FileCSV->write(RowString.toLocal8Bit());

		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(TcpModel->rowCount()));
	}
	FileCSV->close();
	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
	QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Dbf"), MetaTable->LocalListName);
	if (FilePath.isEmpty())
	{
		return false;
	}

	QFileInfo FileInfo(FilePath);
	PathFolder = FileInfo.absolutePath().replace('/', '\\');
	FileLocalName = FileInfo.fileName();

	QSqlDatabase Database = QSqlDatabase::addDatabase("QODBC", ConnectionName);
	Database.setDatabaseName("Driver={Microsoft dBase Driver (*.dbf)};Data Source=" + PathFolder + ';');
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
	QString CreateTableSql = "CREATE TABLE " + PathFolder + '\\' + MetaTable->Name + " (";
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
		QString InsertFields = "INSERT INTO " + PathFolder + '\\' + MetaTable->Name + "(";
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

	QFile File(PathFolder + '\\' + MetaTable->Name.toUpper() + ".dbf");
	if (!File.rename(PathFolder + '\\' + FileLocalName))
	{
		ErrorString = File.errorString();
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExportHTML::ISExportHTML(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent)
	: ISExportWorker(meta_table, tcp_model, parent),
	FileHTML(nullptr)
{

}
//-----------------------------------------------------------------------------
ISExportHTML::~ISExportHTML()
{

}
//-----------------------------------------------------------------------------
bool ISExportHTML::Prepare()
{
	QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Html"), MetaTable->LocalListName);
	if (FilePath.isEmpty())
	{
		return false;
	}

	FileHTML = new QFile(FilePath, this);

	if (FileHTML->exists())
	{
		if (!FileHTML->remove())
		{
			ErrorString = FileHTML->errorString();
			return false;
		}
	}

	if (!FileHTML->open(QIODevice::WriteOnly))
	{
		ErrorString = FileHTML->errorString();
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISExportHTML::Export()
{
	FileHTML->write("<!DOCTYPE HTML>\r\n");
	FileHTML->write("<html>\r\n");
	FileHTML->write(" <head>\r\n");
	FileHTML->write("  <meta charset=\"utf-8\">\r\n");
	FileHTML->write("  <title>" + MetaTable->LocalListName.toUtf8() + "</title>\r\n");
	FileHTML->write("  <style type=\"text/css\">\r\n");
	FileHTML->write("   table, td, th { border-collapse: collapse; border: 1px solid black; }\r\n");
	FileHTML->write("   th { background: lightGray; font-weight: normal; padding: 10px 15px; }\r\n");
	FileHTML->write("   td { border-top: 1px solid #e8edff; padding: 10px 15px; }\r\n");
	FileHTML->write("   tr:nth-child(2n) {background: rgb(240, 240, 240); }\r\n");
	FileHTML->write("  </style>\r\n");
	FileHTML->write(" </head>\r\n");
	FileHTML->write(" <body>\r\n");
	FileHTML->write("  <table>\r\n");
	FileHTML->write("   <caption>" + MetaTable->LocalListName.toUtf8() + "</caption>\r\n");

	if (Header) //Если в экспортируемый файл нужно добавить заголовки колонок
	{
		FileHTML->write("   <tr>\r\n");
		for (const unsigned int &Index : Fields) //Обход выбранных для экспорта полей
		{
			FileHTML->write("    <th>" + TcpModel->GetField(Index).LocalName.toUtf8() + "</th>\r\n");
		}
		FileHTML->write("   </tr>\r\n");
	}

	for (int Row = 0; Row < TcpModel->rowCount(); ++Row) //Обход строк
	{
		if (Canceled) //Если была нажата кнопка "Остановить"
		{
			if (ISMessageBox::ShowQuestion(nullptr, LANG("Export.Process.Cancel"), LANG("Export.Process.Cancel.DetailedText")))
			{
				FileHTML->close();
				QFile::remove(FileHTML->fileName());
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
		QString RowString;

		RowString.append("    <tr>");
		for (const unsigned int &Index : Fields) //Обход колонок
		{
			QVariant Value = Record.Values[Index];
			Value = PrepareValue(TcpModel->GetField(Index).Type, Value);
			RowString.append("<td>" + Value.toString() + "</td>");
		}
		RowString.append("</tr>\r\n");
		FileHTML->write(RowString.toUtf8());

		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(TcpModel->rowCount()));
	}
	FileHTML->write("  </table>\r\n");
	FileHTML->write(" </body>\r\n");
	FileHTML->write("</html>\r\n");
	FileHTML->close();
	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExportJSON::ISExportJSON(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent)
	: ISExportWorker(meta_table, tcp_model, parent),
	FileJSON(nullptr)
{

}
//-----------------------------------------------------------------------------
ISExportJSON::~ISExportJSON()
{

}
//-----------------------------------------------------------------------------
bool ISExportJSON::Prepare()
{
	QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Json"), MetaTable->LocalListName);
	if (FilePath.isEmpty())
	{
		return false;
	}

	FileJSON = new QFile(FilePath, this);
	if (FileJSON->exists())
	{
		if (!FileJSON->remove())
		{
			ErrorString = FileJSON->errorString();
			return false;
		}
	}
	if (!FileJSON->open(QIODevice::WriteOnly))
	{
		ErrorString = FileJSON->errorString();
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISExportJSON::Export()
{
	QVariantList RecordList;
	for (int Row = 0; Row < TcpModel->rowCount(); ++Row) //Обход строк
	{
		if (Canceled) //Если была нажата кнопка "Остановить"
		{
			if (ISMessageBox::ShowQuestion(nullptr, LANG("Export.Process.Cancel"), LANG("Export.Process.Cancel.DetailedText")))
			{
				FileJSON->close();
				QFile::remove(FileJSON->fileName());
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
		QVariantMap RecordMap;
		for (const unsigned int &Index : Fields) //Обход колонок
		{
			RecordMap[TcpModel->GetField(Index).Name] = Record.Values[Index];
		}
		RecordList.append(RecordMap);
		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(TcpModel->rowCount()));
	}
	FileJSON->write(ISSystem::VariantListToJsonString(RecordList));
	FileJSON->close();
	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExportXML::ISExportXML(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent)
	: ISExportWorker(meta_table, tcp_model, parent),
	FileXML(nullptr)
{

}
//-----------------------------------------------------------------------------
ISExportXML::~ISExportXML()
{

}
//-----------------------------------------------------------------------------
bool ISExportXML::Prepare()
{
	QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Xml"), MetaTable->LocalListName);
	if (FilePath.isEmpty())
	{
		return false;
	}

	FileXML = new QFile(FilePath, this);
	if (FileXML->exists())
	{
		if (!FileXML->remove())
		{
			ErrorString = FileXML->errorString();
			return false;
		}
	}
	if (!FileXML->open(QIODevice::WriteOnly))
	{
		ErrorString = FileXML->errorString();
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISExportXML::Export()
{
	QDomDocument DomDocument(MetaTable->Name);
	QDomElement DomElement = DomDocument.createElement(MetaTable->Name);
	DomDocument.appendChild(DomElement);

	for (int Row = 0; Row < TcpModel->rowCount(); ++Row) //Обход строк
	{
		if (Canceled) //Если была нажата кнопка "Остановить"
		{
			if (ISMessageBox::ShowQuestion(nullptr, LANG("Export.Process.Cancel"), LANG("Export.Process.Cancel.DetailedText")))
			{
				FileXML->close();
				QFile::remove(FileXML->fileName());
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

		QDomElement TagRow = DomDocument.createElement(MetaTable->Name);
		ISModelRecord Record = TcpModel->GetRecord(Row); //Текущая строка
		for (const unsigned int &Index : Fields) //Обход колонок
		{
			ISModelField ModelField = TcpModel->GetField(Index);
			QVariant Value = Record.Values[Index];
			Value = PrepareValue(ModelField.Type, Value);
			TagRow.setAttribute(ModelField.Name, Value.toString());
			DomElement.appendChild(TagRow);
		}
		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(TcpModel->rowCount()));
	}
	FileXML->write(DomDocument.toString().toUtf8());
	FileXML->close();
	return true;
}
//-----------------------------------------------------------------------------
