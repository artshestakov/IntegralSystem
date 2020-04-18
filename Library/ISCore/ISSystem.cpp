#include "ISSystem.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
void ISSystem::SetObjectProperty(QObject *Object, const char *PropertyName, const QVariant &PropertyValue)
{
	if (Object)
	{
		Object->setProperty(PropertyName, PropertyValue);
	}
}
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
void ISSystem::SleepMilliseconds(unsigned long Milliseconds)
{
	QThread::msleep(Milliseconds);
}
//-----------------------------------------------------------------------------
void ISSystem::SleepSeconds(unsigned long Seconds)
{
	QThread::sleep(Seconds);
}
//-----------------------------------------------------------------------------
QString ISSystem::GetSizeDir(const QString &DirPath)
{
	QDir Dir(DirPath);
	QFileInfoList FileInfoList = Dir.entryInfoList(QDir::Files);
	qint64 Size = 0;
	for (int i = 0; i < FileInfoList.count(); ++i)
	{
		QFileInfo FileInfo = FileInfoList.at(i);
		QString FilePath = FileInfo.absoluteFilePath();
		qint64 FileSize = GetFileSize(FilePath);
		Size += FileSize;
	}

	QString StringSize = FileSizeFromString(Size);
	return StringSize;
}
//-----------------------------------------------------------------------------
QString ISSystem::FormatQFuncInfo(const QString &QFuncInfo, ISNamespace::FunctionNameFormat Format)
{
	QString Result;
	QString Temp = QFuncInfo;

	switch (Format)
	{
	case ISNamespace::FNF_All:
		Result = QFuncInfo;
		break;

	case ISNamespace::FNF_TypeAndFunction:
		Result = Temp.replace("__thiscall ", QString());
		break;

	case ISNamespace::FNF_Type:
		Result = Temp.replace("__thiscall ", QString()).split("::").at(0);
		break;

	case ISNamespace::FNF_Function:
		Result = Temp.split("::").at(1);
		break;
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISSystem::GetCurrentDayOfWeekName()
{
	return GetDayOfWeekName(static_cast<Qt::DayOfWeek>(QDate::currentDate().dayOfWeek()));
}
//-----------------------------------------------------------------------------
QString ISSystem::GetDayOfWeekName(Qt::DayOfWeek Day)
{
	QString DayOfWeekName = QDate::longDayName(Day);
	BeginSymbolToUpper(DayOfWeekName);
	return DayOfWeekName;
}
//-----------------------------------------------------------------------------
QString ISSystem::GetConfigurationName()
{
#ifdef QT_DEBUG
	return "Debug";
#else
	return "Release";
#endif
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
void ISSystem::RemoveLastSymbolFromString(QString &String, int CountSymbols)
{
	String.chop(CountSymbols);
}
//-----------------------------------------------------------------------------
void ISSystem::RemoveLastSymbolLoop(QString &String, char Char)
{
	while (String.back() == Char)
	{
		String.chop(1);
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
QString ISSystem::SizeToString(const QSize &Size)
{
	return QString(QString::number(Size.height()) + " x " + QString::number(Size.width()));
}
//-----------------------------------------------------------------------------
double ISSystem::MillisecondToSecond(int Milliseconds)
{
	return Milliseconds / 1000;
}
//-----------------------------------------------------------------------------
QVariantMap ISSystem::JsonStringToVariantMap(const QString &JsonString)
{
	return QJsonDocument::fromJson(JsonString.toUtf8()).object().toVariantMap();
}
//-----------------------------------------------------------------------------
ISStringMap ISSystem::JsonStringToStringMap(const QString &JsonString)
{
	QVariantMap VariantMap = JsonStringToVariantMap(JsonString);
	ISStringMap StringMap;
	for (const auto &MapItem : VariantMap.toStdMap())
	{
		StringMap.emplace(MapItem.first, MapItem.second.toString());
	}
	return StringMap;
}
//-----------------------------------------------------------------------------
QString ISSystem::VariantMapToJsonString(const QVariantMap &VariantMap)
{
	return QJsonDocument(QJsonObject::fromVariantMap(VariantMap)).toJson();
}
//-----------------------------------------------------------------------------
QString ISSystem::StringFromMD5(const QString &String)
{
	QCryptographicHash CryptographicHash(QCryptographicHash::Md5);
	CryptographicHash.addData(String.toUtf8());
	QString ResultString(CryptographicHash.result().toHex());
	return ResultString;
}
//-----------------------------------------------------------------------------
qint64 ISSystem::GetFileSize(const QString &FilePath)
{
	return QFileInfo(FilePath).size();
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
QString ISSystem::GetCompleteBaseFileName(const QString &FilePath)
{
	return QFileInfo(FilePath).completeBaseName();
}
//-----------------------------------------------------------------------------
QString ISSystem::GetFileName(const QString &FilePath)
{
	return QFileInfo(FilePath).fileName();
}
//-----------------------------------------------------------------------------
QByteArray ISSystem::GetFileMD5(const QString &FilePath)
{
	QFile File(FilePath);
	if (File.open(QIODevice::ReadOnly))
	{
		QCryptographicHash MD5(QCryptographicHash::Md5);
		MD5.addData(File.readAll());
		File.close();
		return MD5.result();
	}
	return QByteArray();
}
//-----------------------------------------------------------------------------
QDateTime ISSystem::GetCreatedDateFile(const QString &FilePath)
{
	return QFileInfo(FilePath).created();
}
//-----------------------------------------------------------------------------
QDateTime ISSystem::GetLastModifiedFile(const QString &FilePath)
{
	return QFileInfo(FilePath).lastModified();
}
//-----------------------------------------------------------------------------
