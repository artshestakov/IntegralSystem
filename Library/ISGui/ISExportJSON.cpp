#include "ISExportJSON.h"
#include "ISFileDialog.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISExportJSON::ISExportJSON(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent) : ISExportWorker(meta_table, tcp_model, parent)
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
