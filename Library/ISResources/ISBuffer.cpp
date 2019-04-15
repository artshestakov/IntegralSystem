#include "StdAfx.h"
#include "ISBuffer.h"
#include "EXDefines.h"
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
	bool Contains = Animations.contains(AnimationName);
    IS_ASSERT(Contains, QString("Animation \"%1\" not found in buffer animations. File: %2. Line: %3.").arg(AnimationName).arg(SourceFile).arg(FileLine));

	QString AnimationPath = Animations.value(AnimationName);
	QMovie *Movie = new QMovie(AnimationPath, nullptr, parent);
	return Movie;
}
//-----------------------------------------------------------------------------
QIcon ISBuffer::GetIcon(const QString &IconName, const QString &SourceFile, int FileLine)
{
	QIcon Icon;
	if (Icons.contains(IconName))
	{
		Icon = QIcon(Icons.value(IconName));
	}
	else
	{
		IS_ASSERT(false, QString("Icon \"%1\" not found in buffer icons. File: %2. Line: %3.").arg(IconName).arg(SourceFile).arg(FileLine));
	}

	return Icon;
}
//-----------------------------------------------------------------------------
QPixmap ISBuffer::GetPixmap(const QString &PixmapName, const QString &SourceFile, int FileLine)
{
	bool Contains = Pixmaps.contains(PixmapName);
    IS_ASSERT(Contains, QString("Pixmap \"%1\" not found in buffer pixmaps. File: %2. Line: %3.").arg(PixmapName).arg(SourceFile).arg(FileLine));

	QString PixmapPath = Pixmaps.value(PixmapName);
	QPixmap Pixmap(PixmapPath);
	return Pixmap;
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

	ISDebug::ShowDebugString(QString("Buffer animation initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(Animations.count()));
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

		AddImageIcon(FileName, FilePath);
	}

	ISDebug::ShowDebugString(QString("Buffer icons initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(Icons.count()));
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

		AddImage(FileName, FilePath);
	}

	ISDebug::ShowDebugString(QString("Buffer images initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(Pixmaps.count()));
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
	IS_ASSERT(!Animations.count(AnimationName), QString("Animation \"%1\" already exist in buffer animations").arg(AnimationName));

	QFile File(AnimationPath);
	QMovie Movie(&File);
	Movie.setObjectName(AnimationName);
	IS_ASSERT(Movie.isValid(), "Animation invalid: " + AnimationName);
	Animations.insert(AnimationName, AnimationPath);
}
//-----------------------------------------------------------------------------
void ISBuffer::AddImageIcon(const QString &IconName, const QString &IconPath)
{
	IS_ASSERT(!Icons.contains(IconName), QString("Icon \"%1\" already exist in buffer icons").arg(IconName));

	QIcon Icon(IconPath);
	IS_ASSERT(!Icon.isNull(), "Null icon: " + IconName);
	Icons.insert(IconName, IconPath);
}
//-----------------------------------------------------------------------------
void ISBuffer::AddImage(const QString &PixmapName, const QString &PixmapPath)
{
	IS_ASSERT(!Pixmaps.contains(PixmapName), QString("Pixmap \"%1\" already exist in buffer pixmaps").arg(PixmapName));

	QPixmap Pixmap(PixmapPath);
	IS_ASSERT(!Pixmap.isNull(), "Null pixmap: " + PixmapName);
	Pixmaps.insert(PixmapName, PixmapPath);
}
//-----------------------------------------------------------------------------
void ISBuffer::AddAudio(const QString &AudioName, const QString &AudioPath)
{
	bool Contains = Audios.contains(AudioName);
	IS_ASSERT(!Contains, QString("Audio \"%1\" already exist in buffer audios.").arg(AudioName));
	Audios.insert(AudioName, AudioPath);
}
//-----------------------------------------------------------------------------
