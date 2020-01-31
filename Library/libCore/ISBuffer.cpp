#include "ISBuffer.h"
#include "ISDefines.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
ISBuffer::ISBuffer()
{
	
}
//-----------------------------------------------------------------------------
ISBuffer::~ISBuffer()
{

}
//-----------------------------------------------------------------------------
ISBuffer& ISBuffer::GetInstance()
{
	static ISBuffer Buffer;
	return Buffer;
}
//-----------------------------------------------------------------------------
void ISBuffer::Initialize()
{
	InitializeAnimations();
	InitializeIcons();
	InitializePixmaps();
	InitializeAudios();
}
//-----------------------------------------------------------------------------
QMovie* ISBuffer::GetAnimation(const QString &AnimationName, QObject *parent, const QString &SourceFile, int FileLine)
{
	std::map<QString, QMovie *>::const_iterator It = Animations.find(AnimationName);
	if (It == Animations.end())
	{
		return nullptr;
	}
	return It->second;
}
//-----------------------------------------------------------------------------
QIcon ISBuffer::GetIcon(const QString &IconName, const QString &SourceFile, int FileLine)
{
	std::map<QString, QIcon>::const_iterator It = Icons.find(IconName);
	if (It == Icons.end())
	{
		return QIcon();
	}
	return It->second;
}
//-----------------------------------------------------------------------------
QPixmap ISBuffer::GetPixmap(const QString &PixmapName, const QString &SourceFile, int FileLine)
{
	std::map<QString, QPixmap>::const_iterator It = Pixmaps.find(PixmapName);
	if (It == Pixmaps.end())
	{
		return QPixmap(0, 0);
	}
	return It->second;
}
//-----------------------------------------------------------------------------
QString ISBuffer::GetAudio(const QString &AudioName)
{
	return Audios.value(AudioName);
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeAnimations()
{
	ISCountingTime Time;

	QFileInfoList FileInfoList = QDir(":Animation").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		QString FileName = FileInfo.completeBaseName();
		QString FilePath = FileInfo.filePath();

		AddAnimations(FileName, FilePath);
	}

	ISDebug::ShowDebugString(QString("Buffer animation initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(Animations.size()));
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeIcons()
{
	ISCountingTime Time;
	
	QFileInfoList FileInfoList = QDir(":ImageIcons").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		QString FileName = FileInfo.completeBaseName();
		QString FilePath = FileInfo.filePath();

		AddIcon(FileName, FilePath);
	}

	ISDebug::ShowDebugString(QString("Buffer icons initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(Icons.size()));
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializePixmaps()
{
	ISCountingTime Time;
	
	QFileInfoList FileInfoList = QDir(":Images").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		QString FileName = FileInfo.completeBaseName();
		QString FilePath = FileInfo.filePath();

		AddPixmap(FileName, FilePath);
	}

	ISDebug::ShowDebugString(QString("Buffer images initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(Pixmaps.size()));
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeAudios()
{
	ISCountingTime Time;

	QFileInfoList FileInfoList = QDir(":Audio").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		QString FileName = FileInfo.completeBaseName();
		QString FilePath = FileInfo.filePath();

		AddAudio(FileName, FilePath);
	}

	ISDebug::ShowDebugString(QString("Buffer audio initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(Audios.count()));
}
//-----------------------------------------------------------------------------
void ISBuffer::AddAnimations(const QString &AnimationName, const QString &AnimationPath)
{
	IS_ASSERT(Animations.find(AnimationName) == Animations.end(), QString("Animation \"%1\" already exist in buffer animations").arg(AnimationName));
	Animations.emplace(AnimationName, new QMovie(AnimationPath));
}
//-----------------------------------------------------------------------------
void ISBuffer::AddIcon(const QString &IconName, const QString &IconPath)
{
	IS_ASSERT(Icons.find(IconName) == Icons.end(), QString("Icon \"%1\" already exist in buffer icons").arg(IconName));
	Icons.emplace(IconName, QIcon(IconPath));
}
//-----------------------------------------------------------------------------
void ISBuffer::AddPixmap(const QString &PixmapName, const QString &PixmapPath)
{
	IS_ASSERT(Pixmaps.find(PixmapName) == Pixmaps.end(), QString("Pixmap \"%1\" already exist in buffer pixmaps").arg(PixmapName));
	Pixmaps.emplace(PixmapName, QPixmap(PixmapPath));
}
//-----------------------------------------------------------------------------
void ISBuffer::AddAudio(const QString &AudioName, const QString &AudioPath)
{
	bool Contains = Audios.contains(AudioName);
	IS_ASSERT(!Contains, QString("Audio \"%1\" already exist in buffer audios.").arg(AudioName));
	Audios.insert(AudioName, AudioPath);
}
//-----------------------------------------------------------------------------
