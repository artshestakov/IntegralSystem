#include "ISExportXML.h"
#include "ISFileDialog.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISExportXML::ISExportXML(QObject *parent) : ISExportWorker(parent)
{

}
//-----------------------------------------------------------------------------
ISExportXML::~ISExportXML()
{

}
//-----------------------------------------------------------------------------
bool ISExportXML::Prepare()
{
	QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Xml"), LocalName);
	if (!FilePath.length())
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
	QDomDocument DomDocument(TableName);
	QDomElement DomElement = DomDocument.createElement(TableName);
	DomDocument.appendChild(DomElement);

	for (int Row = 0; Row < Model->rowCount(); ++Row) //Обход строк
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

		if (SelectedRows.count()) //Если есть выделенные строки
		{
			if (!SelectedRows.contains(Row))
			{
				continue;
			}
		}

		QDomElement TagRow = DomDocument.createElement(TableName);
		QSqlRecord SqlRecord = Model->GetRecord(Row); //Текущая строка
		for (int Column = 0; Column < Fields.count(); ++Column) //Обход колонок
		{
			QVariant Value = SqlRecord.value(Fields.at(Column)).toString();
			Value = PrepareValue(Value);

			TagRow.setAttribute(Fields.at(Column), Value.toString());
			DomElement.appendChild(TagRow);
		}

		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(Model->rowCount()) + "...");
	}

	QString Content = DomDocument.toString();
	FileXML->write(Content.toUtf8());
	FileXML->close();

	return true;
}
//-----------------------------------------------------------------------------
