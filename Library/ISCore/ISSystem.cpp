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
QString ISSystem::StringToSha256(const QString &String)
{
    return QCryptographicHash::hash(String.toUtf8(), QCryptographicHash::Sha256).toHex().toUpper();
}
//-----------------------------------------------------------------------------
