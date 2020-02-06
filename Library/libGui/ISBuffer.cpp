#include "ISBuffer.h"
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
	std::map<QString, QString>::const_iterator Iterator = Animations.find(AnimationName);
	if (Iterator == Animations.end())
	{
		IS_ASSERT(false, QString("Animation \"%1\" not found in buffer animations. File: %2. Line: %3.").arg(AnimationName).arg(SourceFile).arg(FileLine));
	}
	return new QMovie(Iterator->second, nullptr, parent);
}
//-----------------------------------------------------------------------------
QIcon ISBuffer::GetIcon(const QString &IconName, const QString &SourceFile, int FileLine)
{
	std::map<QString, QIcon>::const_iterator Iterator = Icons.find(IconName);
	if (Iterator == Icons.end())
	{
		IS_ASSERT(false, QString("Icon \"%1\" not found in buffer icons. File: %2. Line: %3.").arg(IconName).arg(SourceFile).arg(FileLine));
	}
	return Iterator->second;
}
//-----------------------------------------------------------------------------
QPixmap ISBuffer::GetPixmap(const QString &PixmapName, const QString &SourceFile, int FileLine)
{
	std::map<QString, QPixmap>::const_iterator Iterator = Pixmaps.find(PixmapName);
	if (Iterator == Pixmaps.end())
	{
		IS_ASSERT(false, QString("Pixmap \"%1\" not found in buffer pixmaps. File: %2. Line: %3.").arg(PixmapName).arg(SourceFile).arg(FileLine));
	}
	return Iterator->second;
}
//-----------------------------------------------------------------------------
QString ISBuffer::GetAudio(const QString &AudioName)
{
	return Audios.find(AudioName)->second;
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeAnimations()
{
	ISCountingTime Time;
	QFileInfoList FileInfoList = QDir(":Animation").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		AddAnimations(FileInfo.completeBaseName(), FileInfo.filePath());
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
		AddImageIcon(FileInfo.completeBaseName(), FileInfo.filePath());
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
		AddImage(FileInfo.completeBaseName(), FileInfo.filePath());
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
		AddAudio(FileInfo.completeBaseName(), FileInfo.filePath());
	}
	ISDebug::ShowDebugString(QString("Buffer audio initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(Audios.size()));
}
//-----------------------------------------------------------------------------
void ISBuffer::AddAnimations(const QString &AnimationName, const QString &AnimationPath)
{
	if (Animations.count(AnimationName))
	{
		IS_ASSERT(false, QString("Animation \"%1\" already exist in buffer animations").arg(AnimationName));
	}
	else
	{
		Animations.emplace(AnimationName, AnimationPath);
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::AddImageIcon(const QString &IconName, const QString &IconPath)
{
	if (Icons.count(IconName))
	{
		IS_ASSERT(false, QString("Icon \"%1\" already exist in buffer icons").arg(IconName));
	}
	else
	{
		Icons.emplace(IconName, QIcon(IconPath));
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::AddImage(const QString &PixmapName, const QString &PixmapPath)
{
	if (Pixmaps.count(PixmapName))
	{
		IS_ASSERT(false, QString("Pixmap \"%1\" already exist in buffer pixmaps").arg(PixmapName));
	}
	else
	{
		Pixmaps.emplace(PixmapName, QPixmap(PixmapPath));
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::AddAudio(const QString &AudioName, const QString &AudioPath)
{
	if (Audios.count(AudioName))
	{
		IS_ASSERT(false, QString("Audio \"%1\" already exist in buffer audios.").arg(AudioName));
	}
	else
	{
		Audios.emplace(AudioName, AudioPath);
	}
}
//-----------------------------------------------------------------------------
