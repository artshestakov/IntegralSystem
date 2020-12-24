#include "ISExportCSV.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISExportCSV::ISExportCSV(PMetaTable *meta_table, QObject *parent)
	: ISExportWorker(meta_table, parent),
	FileCSV(nullptr)
{
	FileCSV = nullptr;
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
		for (const QString &FieldName : Fields) //Обход выбранных для экспорта полей
		{
			if (ISAlgorithm::VectorContains(Fields, FieldName))
			{
				//???
				//HeaderString.append(Model->GetFieldLocalName(FieldName));
				HeaderString.append(';');
			}
		}
		HeaderString.chop(1);
		HeaderString.append("\r\n");
		FileCSV->write(HeaderString.toLocal8Bit());
	}

	for (int Row = 0; Row < /*Model->rowCount()*/0; ++Row) //Обход строк
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

		//???
		//QSqlRecord SqlRecord = Model->GetRecord(Row); //Текущая строка
		QString RowString;

		for (const QString &FieldName : Fields) //Обход колонок
		{
			//???
			//QVariant Value = SqlRecord.value(FieldName);
			//Value = PrepareValue(MetaTable->GetField(FieldName)->Type, Value);
			//RowString.append(Value.toString());
			RowString.append(';');
		}

		RowString.chop(1);
		RowString.append("\r\n");
		FileCSV->write(RowString.toLocal8Bit());

		emit ExportedRow();
		//???
		//emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(Model->rowCount()));
	}

	FileCSV->close();

	return true;
}
//-----------------------------------------------------------------------------
