#include "ISExportCSV.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISExportCSV::ISExportCSV(QObject *parent) : ISExportWorker(parent)
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
	QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Csv"), LocalName);
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

		for (int i = 0; i < Fields.size(); ++i) //Обход выбранных для экспорта полей
		{
			QString FieldName = Fields.at(i);
			if (ISAlgorithm::VectorContains(Fields, FieldName))
			{
				QString LocalName = Model->GetFieldLocalName(FieldName);
				HeaderString.append(LocalName);
				HeaderString.append(';');
			}
		}

		ISSystem::RemoveLastSymbolFromString(HeaderString);
		HeaderString.append("\r\n");
		FileCSV->write(HeaderString.toLocal8Bit());
	}

	for (int Row = 0; Row < Model->rowCount(); ++Row) //Обход строк
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

		QSqlRecord SqlRecord = Model->GetRecord(Row); //Текущая строка
		QString RowString;

		for (int Column = 0; Column < Fields.size(); ++Column) //Обход колонок
		{
			QVariant Value = SqlRecord.value(Fields.at(Column)).toString();
			Value = PrepareValue(Value);
			RowString.append(Value.toString());
			RowString.append(';');
		}

		ISSystem::RemoveLastSymbolFromString(RowString);
		RowString.append("\r\n");
		FileCSV->write(RowString.toLocal8Bit());

		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(Model->rowCount()));
	}

	FileCSV->close();

	return true;
}
//-----------------------------------------------------------------------------
