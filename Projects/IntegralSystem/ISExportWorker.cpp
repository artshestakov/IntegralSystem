#include "ISExportWorker.h"
#include "ISLocalization.h"
#include "ISDialogsCommon.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISExportWorker::ISExportWorker(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent)
    : QObject(parent),
    ErrorString(LANG("Export.Error.NoError")),
    TableName(table_name),
    TableLocalListName(table_local_list_name),
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
        if (Type == ISNamespace::FieldType::Bool)
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
ISExportCSV::ISExportCSV(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent)
    : ISExportWorker(table_name, table_local_list_name, tcp_model, parent),
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
    QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Csv"), TableLocalListName);
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
        emit Message(ISAlgorithm::CStringF(LANG("Export.Process.Process"), Row + 1, TcpModel->rowCount()));
    }
    FileCSV->close();
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExportHTML::ISExportHTML(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent)
    : ISExportWorker(table_name, table_local_list_name, tcp_model, parent),
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
    QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Html"), TableLocalListName);
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
    FileHTML->write("  <title>" + TableLocalListName.toUtf8() + "</title>\r\n");
    FileHTML->write("  <style type=\"text/css\">\r\n");
    FileHTML->write("   table, td, th { border-collapse: collapse; border: 1px solid black; }\r\n");
    FileHTML->write("   th { background: lightGray; font-weight: normal; padding: 10px 15px; }\r\n");
    FileHTML->write("   td { border-top: 1px solid #e8edff; padding: 10px 15px; }\r\n");
    FileHTML->write("   tr:nth-child(2n) {background: rgb(240, 240, 240); }\r\n");
    FileHTML->write("  </style>\r\n");
    FileHTML->write(" </head>\r\n");
    FileHTML->write(" <body>\r\n");
    FileHTML->write("  <table>\r\n");
    FileHTML->write("   <caption>" + TableLocalListName.toUtf8() + "</caption>\r\n");

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
        emit Message(ISAlgorithm::CStringF(LANG("Export.Process.Process"), Row + 1, TcpModel->rowCount()));
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
ISExportJSON::ISExportJSON(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent)
    : ISExportWorker(table_name, table_local_list_name, tcp_model, parent),
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
    QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Json"), TableLocalListName);
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
        emit Message(ISAlgorithm::CStringF(LANG("Export.Process.Process"), Row + 1, TcpModel->rowCount()));
    }
    FileJSON->write(ISGui::VariantListToJsonString(RecordList));
    FileJSON->close();
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExportXML::ISExportXML(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent)
    : ISExportWorker(table_name, table_local_list_name, tcp_model, parent),
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
    QString FilePath = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Xml"), TableLocalListName);
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
    QDomDocument DomDocument(TableName);
    QDomElement DomElement = DomDocument.createElement(TableName);
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

        QDomElement TagRow = DomDocument.createElement(TableName);
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
        emit Message(ISAlgorithm::CStringF(LANG("Export.Process.Process"), Row + 1, TcpModel->rowCount()));
    }
    FileXML->write(DomDocument.toString().toUtf8());
    FileXML->close();
    return true;
}
//-----------------------------------------------------------------------------
