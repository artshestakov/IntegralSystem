#include "ISExportXML.h"
#include "ISFileDialog.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISExportXML::ISExportXML(PMetaTable *meta_table, QObject *parent) : ISExportWorker(meta_table, parent)
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

		if (!SelectedRows.empty()) //Если есть выделенные строки
		{
			if (!ISAlgorithm::VectorContains(SelectedRows, Row))
			{
				continue;
			}
		}

		QDomElement TagRow = DomDocument.createElement(MetaTable->Name);
		QSqlRecord SqlRecord = Model->GetRecord(Row); //Текущая строка
		for (const QString &FieldName : Fields) //Обход колонок
		{
			QVariant Value = SqlRecord.value(FieldName).toString();
			Value = PrepareValue(MetaTable->GetField(FieldName)->Type, Value);
			TagRow.setAttribute(FieldName, Value.toString());
			DomElement.appendChild(TagRow);
		}
		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(Model->rowCount()));
	}
	FileXML->write(DomDocument.toString().toUtf8());
	FileXML->close();
	return true;
}
//-----------------------------------------------------------------------------
