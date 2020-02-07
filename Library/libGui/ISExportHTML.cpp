#include "ISExportHTML.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISExportHTML::ISExportHTML(QObject *parent) : ISExportWorker(parent)
{
	FileHTML = nullptr;
}
//-----------------------------------------------------------------------------
ISExportHTML::~ISExportHTML()
{

}
//-----------------------------------------------------------------------------
bool ISExportHTML::Prepare()
{
	QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Html"), LocalName);
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
	FileHTML->write("  <title>" + LocalName.toUtf8() + "</title>\r\n");
	FileHTML->write("  <style type=\"text/css\">\r\n");
	FileHTML->write("   table, td, th { border-collapse: collapse; border: 1px solid black; }\r\n");
	FileHTML->write("   th { background: lightGray; font-weight: normal; padding: 10px 15px; }\r\n");
	FileHTML->write("   td { border-top: 1px solid #e8edff; padding: 10px 15px; }\r\n");
	FileHTML->write("   tr:nth-child(2n) {background: rgb(240, 240, 240); }\r\n");
	FileHTML->write("  </style>\r\n");
	FileHTML->write(" </head>\r\n");
	FileHTML->write(" <body>\r\n");
	FileHTML->write("  <table>\r\n");
	FileHTML->write("   <caption>" + LocalName.toUtf8() + "</caption>\r\n");

	if (Header) //���� � �������������� ���� ����� �������� ��������� �������
	{
		FileHTML->write("   <tr>\r\n");

		for (int i = 0; i < Fields.count(); ++i) //����� ��������� ��� �������� �����
		{
			QString FieldName = Fields.at(i);
			if (Fields.contains(FieldName))
			{
				QString LocalName = Model->GetFieldLocalName(FieldName);
				FileHTML->write("    <th>" + LocalName.toUtf8() + "</th>\r\n");
			}
		}

		FileHTML->write("   </tr>\r\n");
	}

	for (int Row = 0; Row < Model->rowCount(); ++Row) //����� �����
	{
		if (Canceled) //���� ���� ������ ������ "����������"
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

		if (SelectedRows.count()) //���� ���� ���������� ������
		{
			if (!SelectedRows.contains(Row))
			{
				continue;
			}
		}

		QSqlRecord SqlRecord = Model->GetRecord(Row); //������� ������
		QString RowString;

		RowString.append("    <tr>");
		for (int Column = 0; Column < Fields.count(); ++Column) //����� �������
		{
			QVariant Value = SqlRecord.value(Fields.at(Column)).toString();
			Value = PrepareValue(Value);
			RowString.append("<td>" + Value.toString().toUtf8() + "</td>");
		}
		RowString.append("</tr>\r\n");

		FileHTML->write(RowString.toUtf8());

		emit ExportedRow();
		emit Message(LANG("Export.Process.Process").arg(Row + 1).arg(Model->rowCount()) + "...");
	}

	FileHTML->write("  </table>\r\n");
	FileHTML->write(" </body>\r\n");
	FileHTML->write("</html>\r\n");

	return true;
}
//-----------------------------------------------------------------------------
