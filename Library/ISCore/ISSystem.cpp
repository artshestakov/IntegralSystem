#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
QDomElement ISSystem::GetDomElement(QFile &File)
{
    QDomDocument XmlDocument;
    XmlDocument.setContent(&File);
    QDomElement DomElement = XmlDocument.documentElement();
    return DomElement;
}
//-----------------------------------------------------------------------------
QDomElement ISSystem::GetDomElement(const QString &Content)
{
    QDomDocument XmlDocument;
    XmlDocument.setContent(Content);
    QDomElement DomElement = XmlDocument.documentElement();
    return DomElement;
}
//-----------------------------------------------------------------------------
void ISSystem::BeginSymbolToUpper(QString &String)
{
    if (!String.isEmpty())
    {
        String[0] = String[0].toUpper();
    }
}
//-----------------------------------------------------------------------------
void ISSystem::ExecLoop(unsigned long Milliseconds)
{
    QEventLoop EventLoop;
    QTimer::singleShot(Milliseconds, &EventLoop, &QEventLoop::quit);
    EventLoop.exec();
}
//-----------------------------------------------------------------------------
bool ISSystem::CreateDir(const QString &DirPath)
{
    QString ErrorString;
    return CreateDir(DirPath, ErrorString);
}
//-----------------------------------------------------------------------------
bool ISSystem::CreateDir(const QString &DirPath, QString &ErrorString)
{
    QDir Dir(DirPath);
    bool Result = Dir.exists();
    if (!Result)
    {
        Result = Dir.mkdir(DirPath);
        if (!Result)
        {
            ErrorString = "Error creting dir with path: " + DirPath;
        }
    }
    return Result;
}
//-----------------------------------------------------------------------------
QVariantMap ISSystem::JsonStringToVariantMap(const QString &JsonString, QJsonParseError &JsonParseError)
{
    QVariantMap VariantMap;
    if (!JsonString.isEmpty())
    {
        QJsonDocument JsonDocument = QJsonDocument::fromJson(JsonString.toUtf8(), &JsonParseError);
        if (JsonParseError.error == QJsonParseError::NoError)
        {
            VariantMap = JsonDocument.object().toVariantMap();
        }
    }
    return VariantMap;
}
//-----------------------------------------------------------------------------
QByteArray ISSystem::VariantMapToJsonString(const QVariantMap &VariantMap, QJsonDocument::JsonFormat Format)
{
    return QJsonDocument(QJsonObject::fromVariantMap(VariantMap)).toJson(Format);
}
//-----------------------------------------------------------------------------
QByteArray ISSystem::VariantListToJsonString(const QVariantList &VariantList, QJsonDocument::JsonFormat Format)
{
	return QJsonDocument(QJsonArray::fromVariantList(VariantList)).toJson(Format);
}
//-----------------------------------------------------------------------------
QString ISSystem::StringToMD5(const QString &String)
{
    QCryptographicHash CryptographicHash(QCryptographicHash::Md5);
    CryptographicHash.addData(String.toUtf8());
    return QString(CryptographicHash.result().toHex());
}
//-----------------------------------------------------------------------------
QString ISSystem::StringToSha256(const QString &String)
{
    return QCryptographicHash::hash(String.toUtf8(), QCryptographicHash::Sha256).toHex().toUpper();
}
//-----------------------------------------------------------------------------
QString ISSystem::FileSizeFromString(qint64 FileSize)
{
    qint64 Size = FileSize;
    int i = 0;
    for (; Size > 1023; Size /= 1024, ++i) { }
    QString String = QString().setNum(Size) + "BKMGT"[i];

    if (String.contains("B"))
    {
        String.replace("B", " B");
    }
    else if (String.contains("K"))
    {
        String.replace("K", " Kb");
    }
    else if (String.contains("M"))
    {
        String.replace("M", " Mb");
    }
    else if (String.contains("G"))
    {
        String.replace("G", " Gb");
    }
    else if (String.contains("T"))
    {
        String.replace("T", " Tb");
    }

    return String;
}
//-----------------------------------------------------------------------------
