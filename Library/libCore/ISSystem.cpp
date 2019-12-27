#include "StdAfx.h"
#include "ISSystem.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISDebug.h"
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
	for (int i = 0; i < FileInfoList.count(); i++)
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
	QString DayOfWeekName = QDate::longDayName(DATE_TODAY.dayOfWeek());
	BeginSymbolToUpper(DayOfWeekName);
	return DayOfWeekName;
}
//-----------------------------------------------------------------------------
QString ISSystem::GetDayOfWeekName(Qt::DayOfWeek Day)
{
	QString DayOfWeekName = QDate::longDayName(Day);
	DayOfWeekName.replace(0, 1, DayOfWeekName.at(0).toUpper()); //Установка первой буквы в верхний регистр
	return DayOfWeekName;
}
//-----------------------------------------------------------------------------
QString ISSystem::GetConfigurationName()
{
	QString Configuration;

#ifdef QT_DEBUG
	Configuration = "Debug";
#else
	Configuration = "Release";
#endif

	return Configuration;
}
//-----------------------------------------------------------------------------
bool ISSystem::IsConfigurationDebug()
{
#ifdef QT_DEBUG
	return true;
#else
	return false;
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
		QFile File(DirPath + "/" + *FileIterator);
		File.remove();
		++FileIterator;
	}

	QStringList Dirs = Dir.entryList(QDir::Dirs);
	QStringList::Iterator DirIterator = Dirs.begin();
	while (DirIterator != Dirs.end())
	{
		if (*DirIterator != "." && *DirIterator != "..")
		{
			ClearDirRecursive(DirPath + "/" + *DirIterator);
		}

		++DirIterator;
	}

	Dir.rmdir(DirPath);
}
//-----------------------------------------------------------------------------
bool ISSystem::LoadModule(const QString &ModulePath, QString &ErrorString)
{
	bool Result = false;

	QLibrary Library(ModulePath);
	Result = Library.load();

	if (!Result)
	{
		ErrorString = Library.errorString();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISSystem::RemoveLastSymbolFromString(QString &String, int CountSymbols)
{
	String.chop(CountSymbols);
}
//-----------------------------------------------------------------------------
bool ISSystem::OpenFolder(const QString &FolderPath)
{
	return QDesktopServices::openUrl(QUrl(FolderPath));
}
//-----------------------------------------------------------------------------
void ISSystem::RepaintWidget(QWidget *Widget, bool adjust_size)
{
	if (Widget)
	{
		if (adjust_size)
		{
			Widget->adjustSize();
		}

		Widget->repaint();
	}
}
//-----------------------------------------------------------------------------
void ISSystem::ProcessEvents()
{
	qApp->processEvents();
}
//-----------------------------------------------------------------------------
void ISSystem::MoveWidgetToDesktop(QWidget *Widget, ISNamespace::MoveWidgetDesktop MovePosition)
{
	int X = 0;
	int Y = 0;

	if (MovePosition == ISNamespace::MWD_LeftUp)
	{
		X = 30;
		Y = 30;
	}
	else if (MovePosition == ISNamespace::MWD_LeftBottom)
	{
		X = 30;

		Y = QApplication::desktop()->availableGeometry().height();
        Y = Y - Widget->height();
		Y = Y - 30;
	}
	else if (MovePosition == ISNamespace::MWD_RightUp)
	{
		X = QApplication::desktop()->availableGeometry().width();
		X = X - Widget->width();
		X = X - 30;

		Y = 30;
	}
	else if (MovePosition == ISNamespace::MWD_RightBottom)
	{
		X = QApplication::desktop()->availableGeometry().width();
		X = X - Widget->width();
		X = X - 30;

		Y = QApplication::desktop()->availableGeometry().height();
		Y = Y - Widget->height();
		Y = Y - 30;
	}
	else if (MovePosition == ISNamespace::MWD_Center)
	{
		QRect Rect = Widget->frameGeometry();
		Rect.moveCenter(QDesktopWidget().availableGeometry().center());

		X = Rect.topLeft().x();
		Y = Rect.topLeft().y();
	}

	Widget->move(X, Y);
}
//-----------------------------------------------------------------------------
void ISSystem::SetWaitGlobalCursor(bool Wait)
{
	if (Wait)
	{
		QApplication::setOverrideCursor(CURSOR_WAIT);
	}
	else
	{
		QApplication::restoreOverrideCursor();

		if (QApplication::overrideCursor())
		{
			if (QApplication::overrideCursor()->shape() == CURSOR_WAIT)
			{
				QApplication::restoreOverrideCursor();
			}
		}
	}
}
//-----------------------------------------------------------------------------
ISNamespace::ApplicationType ISSystem::GetApplicationType()
{
	QObject *ApplicationObject = qobject_cast<QApplication*>(QCoreApplication::instance());
	if (ApplicationObject)
	{
		return ISNamespace::AT_GUI;
	}
	else
	{
		return ISNamespace::AT_CONSOLE;
	}

	return ISNamespace::AT_UNKNOWN;
}
//-----------------------------------------------------------------------------
ISUuid ISSystem::GenerateUuid()
{
	return QUuid::createUuid();
}
//-----------------------------------------------------------------------------
QString ISSystem::PrepareLongToolTip(QString ToolTipText, int MaxWidth)
{
	QFontMetrics FontMetrics(QToolTip::font());
	QString Result;

	for (;;)
	{
		int i = 0;
		while (i < ToolTipText.length())
		{
			if (FontMetrics.width(ToolTipText.left(++i + 1)) > MaxWidth)
			{
				int j = ToolTipText.lastIndexOf(' ', i);
				if (j > 0)
				{
					i = j;
				}

				Result += ToolTipText.left(i);
				Result += '\n';
				ToolTipText = ToolTipText.mid(i + 1);
				break;
			}
		}

		if (i >= ToolTipText.length())
		{
			break;
		}
	}

	return Result + ToolTipText;
}
//-----------------------------------------------------------------------------
void ISSystem::BeginSymbolToUpper(QString &String)
{
	if (String.length())
	{
		String = String.replace(0, 1, String.at(0).toUpper());
	}
}
//-----------------------------------------------------------------------------
QString ISSystem::GetCurrentOSName()
{
#if defined(Q_OS_WIN)
	return OS_NAME_WINDOWS;
#elif defined(Q_OS_LINUX)
	return OS_NAME_LINUX;
#elif defined(Q_OS_MACX)
	return OS_NAME_MACOS;
#endif
	return OS_NAME_UNKNOWN;
}
//-----------------------------------------------------------------------------
ISNamespace::OSType ISSystem::GetCurrentOSType()
{
#if defined(Q_OS_WIN)
    return ISNamespace::OST_Windows;
#elif defined(Q_OS_LINUX)
    return ISNamespace::OST_Linux;
#elif defined(Q_OS_MACX)
    return ISNamespace::OST_MACOS;
#endif
	return ISNamespace::OST_Unknown;
}
//-----------------------------------------------------------------------------
QString ISSystem::GetLibraryExtension()
{
	QString Extension;
    if (GetCurrentOSType() == ISNamespace::OST_Windows)
	{
		Extension = EXTENSION_DLL;
	}
    else if (GetCurrentOSType() == ISNamespace::OST_Linux)
	{
		Extension = EXTENSION_SO;
	}

	return Extension;
}
//-----------------------------------------------------------------------------
bool ISSystem::CheckExistSlot(QObject *Object, const QString &SlotName)
{
	for (int i = 0; i < Object->metaObject()->methodCount(); i++)
	{
		if (Object->metaObject()->method(i).name() == SlotName)
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
bool ISSystem::LoadResource()
{
	QString Path;
	switch (GetCurrentOSType())
	{
	case ISNamespace::OST_Windows: Path = APPLICATION_DIR_PATH + "/ISResources.dll"; break;
	case ISNamespace::OST_Linux: Path = APPLICATION_DIR_PATH + "/libISResources.so"; break;
	}

	QString ErrorString;
	bool Result = ISSystem::LoadModule(Path, ErrorString);
	if (!Result)
	{
		ISDebug::ShowWarningString(ErrorString);
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
	return AddressString.contains(SYMBOL_FIAS_SPLIT);
}
//-----------------------------------------------------------------------------
bool ISSystem::CheckPressCapsLook()
{
	bool Result = true;

#ifdef WIN32
	if (GetKeyState(VK_CAPITAL) == 1)
	{
		Result = true;
	}
	else
	{
		Result = false;
	}
#endif

	return Result;
}
//-----------------------------------------------------------------------------
bool ISSystem::IsStringUrl(const QString &Url)
{
	bool Result = false;
	int Pos = 0;
	QString Temp = Url;
	if (QRegExpValidator(QRegExp(REG_EXP_URL)).validate(Temp, Pos) == QValidator::Acceptable)
	{
		Result = true;
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString ISSystem::GetCurrentLayoutName()
{
	QString Result;

#ifdef WIN32
	char LayoutName[KL_NAMELENGTH];
	GetKeyboardLayoutNameA(LayoutName);
	if (atoi(LayoutName) == 409)
	{
		Result = "ENG";
	}
	else if (atoi(LayoutName) == 419)
	{
		Result = "RUS";
	}
#endif

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
	QString FormattedTime;
	int hours = Milliseconds / (1000 * 60 * 60);
	int minutes = (Milliseconds - (hours * 1000 * 60 * 60)) / (1000 * 60);
	int seconds = (Milliseconds - (minutes * 1000 * 60) - (hours * 1000 * 60 * 60)) / 1000;
	int milliseconds = Milliseconds - (seconds * 1000) - (minutes * 1000 * 60) - (hours * 1000 * 60 * 60);
	FormattedTime.append(QString("%1").arg(hours, 2, 10, QLatin1Char('0')) + ":" + QString("%1").arg(minutes, 2, 10, QLatin1Char('0')) + ":" + QString("%1").arg(seconds, 2, 10, QLatin1Char('0')) + ":" + QString("%1").arg(milliseconds, 3, 10, QLatin1Char('0')));
	return FormattedTime;
}
//-----------------------------------------------------------------------------
void ISSystem::SetFontWidgetUnderline(QWidget *Widget, bool Underline)
{
	QFont Font = Widget->font();
	Font.setUnderline(Underline);
	Widget->setFont(Font);
}
//-----------------------------------------------------------------------------
void ISSystem::SetFontWidgetItalic(QWidget *Widget, bool Italic)
{
	QFont Font = Widget->font();
	Font.setItalic(Italic);
	Widget->setFont(Font);
}
//-----------------------------------------------------------------------------
void ISSystem::SetFontWidgetBold(QWidget *Widget, bool Bold)
{
	QFont Font = Widget->font();
	Font.setBold(Bold);
	Widget->setFont(Font);
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
QFont ISSystem::StringToFont(const QString &FontText)
{
	QFont FontReturned;
	if (FontText.length())
	{
		FontReturned = qvariant_cast<QFont>(FontText);
	}
	else
	{
		FontReturned = FONT_APPLICATION_STRING;
	}

	return FontReturned;
}
//-----------------------------------------------------------------------------
QByteArray ISSystem::IconToByteArray(const QIcon &Icon)
{
	QPixmap PixmapIcon = Icon.pixmap(SIZE_16_16);
	QByteArray ByteArray;
	QBuffer Buffer(&ByteArray);
	if (Buffer.open(QIODevice::WriteOnly))
	{
		PixmapIcon.save(&Buffer, "PNG");
	}
	return ByteArray;
}
//-----------------------------------------------------------------------------
QIcon ISSystem::ByteArrayToIcon(const QByteArray &ByteArray)
{
	QIcon Icon;
	QPixmap Pixmap;
	if (Pixmap.loadFromData(ByteArray, "PNG"))
	{
		Icon = QIcon(Pixmap);
	}
	return Icon;
}
//-----------------------------------------------------------------------------
QByteArray ISSystem::PixmapToByteArray(const QPixmap &Pixmap)
{
	QPixmap ConvertingPixmap = Pixmap;
	QByteArray ByteArray;
	QBuffer Buffer(&ByteArray);
    if (Buffer.open(QIODevice::WriteOnly));
	{
		ConvertingPixmap.save(&Buffer, "PNG");
	}
	return ByteArray;
}
//-----------------------------------------------------------------------------
QPixmap ISSystem::ByteArrayToPixmap(const QByteArray &ByteArray)
{
	QPixmap Pixmap;
    Pixmap.loadFromData(ByteArray);
	return Pixmap;
}
//-----------------------------------------------------------------------------
QString ISSystem::SizeToString(const QSize &Size)
{
	int Height = Size.height();
	int Width = Size.width();

	QString HeightString = QString::number(Height);
	QString WidthString = QString::number(Width);

	QString SizeString = HeightString + " x " + WidthString;
	return SizeString;
}
//-----------------------------------------------------------------------------
QColor ISSystem::StringToColor(const QString &String)
{
	QColor Color(String);
	return Color;
}
//-----------------------------------------------------------------------------
double ISSystem::MillisecondToSecond(int Milliseconds)
{
	double Seconds = Milliseconds / 1000;
	return Seconds;
}
//-----------------------------------------------------------------------------
QVariantMap ISSystem::JsonStringToVariantMap(const QString &JsonString)
{
	return QJsonDocument::fromJson(JsonString.toUtf8()).object().toVariantMap();
}
//-----------------------------------------------------------------------------
QStringMap ISSystem::JsonStringToStringMap(const QString &JsonString)
{
	QVariantMap VariantMap = JsonStringToVariantMap(JsonString);
	QStringMap StringMap;
	for (const auto &MapItem : VariantMap.toStdMap())
	{
		StringMap.insert(MapItem.first, MapItem.second.toString());
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
	QFileInfo FileInfo(FilePath);
	qint64 Size = FileInfo.size();
	return Size;
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
bool ISSystem::OpenUrl(const QString &Url)
{
	SetWaitGlobalCursor(true);
	bool Result = QDesktopServices::openUrl(QUrl(Url));
	SetWaitGlobalCursor(false);
	return Result;
}
//-----------------------------------------------------------------------------
bool ISSystem::OpenFile(const QString &FilePath)
{
	SetWaitGlobalCursor(true);
	bool Result = QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(FilePath)));
	SetWaitGlobalCursor(false);
	return Result;
}
//-----------------------------------------------------------------------------
QIcon ISSystem::GetIconFile(const QString &FilePath)
{
	QFileInfo FileInfo(FilePath);
	QFileIconProvider FileIconProvider;
	return FileIconProvider.icon(FileInfo);
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
	QFileInfo FileInfo(FilePath);
	QDateTime DateTime = FileInfo.created();
	return DateTime;
}
//-----------------------------------------------------------------------------
QDateTime ISSystem::GetLastModifiedFile(const QString &FilePath)
{
	QFileInfo FileInfo(FilePath);
	QDateTime DateTime = FileInfo.lastModified();
	return DateTime;
}
//-----------------------------------------------------------------------------
