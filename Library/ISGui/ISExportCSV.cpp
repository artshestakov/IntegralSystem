#include "ISExportCSV.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
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
	if (!FilePath.length())
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
