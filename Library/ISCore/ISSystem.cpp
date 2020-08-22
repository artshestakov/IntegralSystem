#include "ISSystem.h"
#include "ISDefinesCore.h"
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
QString ISSystem::GetSizeDir(const QString &DirPath)
{
	qint64 Size = 0;
	QFileInfoList FileInfoList = QDir(DirPath).entryInfoList(QDir::Files);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		Size += ISAlgorithm::GetFileSize(FileInfo.absoluteFilePath().toStdString());
	}
	return FileSizeFromString(Size);
}
//-----------------------------------------------------------------------------
QString ISSystem::GetDayOfWeekName(Qt::DayOfWeek Day)
{
	QString DayOfWeekName = QDate::longDayName(Day);
	BeginSymbolToUpper(DayOfWeekName);
	return DayOfWeekName;
}
//-----------------------------------------------------------------------------
void ISSystem::ClearDirRecursive(const QString &DirPath)
{
	QDir Dir(DirPath);
	QStringList Files = Dir.entryList(QDir::Files);

	QStringList::Iterator FileIterator = Files.begin();
	while (FileIterator != Files.end())
	{
		QFile File(DirPath + '/' + *FileIterator);
		File.remove();
		++FileIterator;
	}

	QStringList Dirs = Dir.entryList(QDir::Dirs);
	QStringList::Iterator DirIterator = Dirs.begin();
	while (DirIterator != Dirs.end())
	{
		if (*DirIterator != SYMBOL_POINT && *DirIterator != "..")
		{
			ClearDirRecursive(DirPath + '/' + *DirIterator);
		}
		++DirIterator;
	}

	Dir.rmdir(DirPath);
}
//-----------------------------------------------------------------------------
void ISSystem::RemoveLastSymbolLoop(QString &String, char Char)
{
	if (!String.isEmpty())
	{
		while (String.back() == Char)
		{
			String.chop(1);
		}
	}
}
//-----------------------------------------------------------------------------
ISUuid ISSystem::GenerateUuid()
{
	return QUuid::createUuid();
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
bool ISSystem::CheckExistSlot(QObject *Object, const QString &SlotName)
{
	bool Result = false;
	for (int i = 0; i < Object->metaObject()->methodCount(); ++i)
	{
		Result = Object->metaObject()->method(i).name() == SlotName;
		if (Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISSystem::ExecLoop(unsigned long Milliseconds)
{
	QEventLoop EventLoop;
	QTimer::singleShot(Milliseconds, &EventLoop, &QEventLoop::quit);
	EventLoop.exec();
}
//-----------------------------------------------------------------------------
bool ISSystem::AddressIsList(const QString &AddressString)
{
	return AddressString.contains(ISDefines::Core::SYMBOL_FIAS_SPLIT);
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
void ISSystem::ProcessEvents()
{
	qApp->processEvents();
}
//-----------------------------------------------------------------------------
bool ISSystem::IsConfigurator()
{
	return ISDefines::Core::APPLICATION_NAME == "Configurator";
}
//-----------------------------------------------------------------------------
int ISSystem::TimeFromMinutes(const QTime &Time)
{
	int Minute = 0;
	if (Time.hour())
	{
		Minute += Time.hour() * 60;
	}
	if (Time.minute())
	{
		Minute += Time.minute();
	}
	return Minute;
}
//-----------------------------------------------------------------------------
QString ISSystem::MillisecondsToString(int Milliseconds)
{
	int hours = Milliseconds / (1000 * 60 * 60);
	int minutes = (Milliseconds - (hours * 1000 * 60 * 60)) / (1000 * 60);
	int seconds = (Milliseconds - (minutes * 1000 * 60) - (hours * 1000 * 60 * 60)) / 1000;
	int milliseconds = Milliseconds - (seconds * 1000) - (minutes * 1000 * 60) - (hours * 1000 * 60 * 60);
	return QString().append(QString("%1").arg(hours, 2, 10, QLatin1Char('0')) + ':' + QString("%1").arg(minutes, 2, 10, QLatin1Char('0')) + ':' + QString("%1").arg(seconds, 2, 10, QLatin1Char('0')) + ':' + QString("%1").arg(milliseconds, 3, 10, QLatin1Char('0')));
}
//-----------------------------------------------------------------------------
QString ISSystem::StringToBase64(const QString &String)
{
	QByteArray ByteArray = String.toUtf8();
	QByteArray StringBase64 = ByteArray.toBase64();
	return QString(StringBase64);
}
//-----------------------------------------------------------------------------
QString ISSystem::Base64ToString(const QString &Base64)
{
	QString String = QByteArray::fromBase64(Base64.toUtf8());
	return String;
}
//-----------------------------------------------------------------------------
QVariantMap ISSystem::JsonStringToVariantMap(const QString &JsonString, QString *ErrorString)
{
	QVariantMap VariantMap;
	if (!JsonString.isEmpty())
	{
		QJsonParseError JsonParseError;
		QJsonDocument JsonDocument = QJsonDocument::fromJson(JsonString.toUtf8(), &JsonParseError);
		if (JsonParseError.error == QJsonParseError::NoError)
		{
			QJsonObject JsonObject = JsonDocument.object();
			VariantMap = JsonObject.toVariantMap();
		}
		else
		{
            *ErrorString = JsonParseError.errorString();
		}
	}
	else
	{
        *ErrorString = "Empty string";
	}
	return VariantMap;
}
//-----------------------------------------------------------------------------
ISStringMap ISSystem::JsonStringToStringMap(const QString &JsonString, QString *ErrorString)
{
	QVariantMap VariantMap = JsonStringToVariantMap(JsonString, ErrorString);
	ISStringMap StringMap;
	for (const auto &MapItem : VariantMap.toStdMap())
	{
		StringMap.emplace(MapItem.first, MapItem.second.toString());
	}
	return StringMap;
}
//-----------------------------------------------------------------------------
QString ISSystem::VariantMapToJsonString(const QVariantMap &VariantMap, QJsonDocument::JsonFormat Format)
{
	return QJsonDocument(QJsonObject::fromVariantMap(VariantMap)).toJson(Format);
}
//-----------------------------------------------------------------------------
QString ISSystem::StringToMD5(const QString &String)
{
	QCryptographicHash CryptographicHash(QCryptographicHash::Md5);
	CryptographicHash.addData(String.toUtf8());
	return QString(CryptographicHash.result().toHex());
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
