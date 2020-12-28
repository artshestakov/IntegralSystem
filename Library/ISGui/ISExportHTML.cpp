#include "ISExportHTML.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
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
	if (!FilePath.length())
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
