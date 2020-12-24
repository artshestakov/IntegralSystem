#include "ISBuffer.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISBuffer::ISBuffer()
{
	
}
//-----------------------------------------------------------------------------
ISBuffer::~ISBuffer()
{

}
//-----------------------------------------------------------------------------
ISBuffer& ISBuffer::Instance()
{
	static ISBuffer Buffer;
	return Buffer;
}
//-----------------------------------------------------------------------------
QVariantList ISBuffer::GetTaskPriority() const
{
	return TaskPriority;
}
//-----------------------------------------------------------------------------
void ISBuffer::SetTaskPriority(const QVariantList &task_priority)
{
	TaskPriority = task_priority;
}
//-----------------------------------------------------------------------------
void ISBuffer::Initialize()
{
	InitializeAnimations();
	InitializeIcons();
	InitializePixmaps();
	InitializeAudios();
	InitializeStyleSheets();
	InitializeSqlQueryes();
}
//-----------------------------------------------------------------------------
QMovie* ISBuffer::GetAnimation(const QString &AnimationName, QObject *parent, const char *SourceFile, int FileLine)
{
	ISStringMap::const_iterator Iterator = Animations.find(AnimationName);
	if (Iterator == Animations.end())
	{
		IS_ASSERT(false, QString("Animation \"%1\" not found in buffer animations. File: %2. Line: %3.").arg(AnimationName).arg(SourceFile).arg(FileLine));
	}
	return new QMovie(Iterator->second, nullptr, parent);
}
//-----------------------------------------------------------------------------
QIcon ISBuffer::GetIcon(const QString &IconName, const char *SourceFile, int FileLine)
{
	std::map<QString, QIcon>::const_iterator Iterator = Icons.find(IconName);
	if (Iterator == Icons.end())
	{
		IS_ASSERT(false, QString("Icon \"%1\" not found in buffer icons. File: %2. Line: %3.").arg(IconName).arg(SourceFile).arg(FileLine));
	}
	return Iterator->second;
}
//-----------------------------------------------------------------------------
QPixmap ISBuffer::GetPixmap(const QString &PixmapName, const char *SourceFile, int FileLine)
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
QString ISBuffer::GetStyle(const QString &StyleName, const char *SourceFile, int FileLine) const
{
	ISStringMap::const_iterator Iterator = StyleSheets.find(StyleName);
	if (Iterator == StyleSheets.end())
	{
		IS_ASSERT(false, QString("StyleSheet \"%1\" not found. File: %2; Line: %3").arg(StyleName).arg(SourceFile).arg(FileLine));
	}
	return Iterator->second;
}
//-----------------------------------------------------------------------------
QString ISBuffer::GetSQL(const QString &QueryName, const char *SourceFile, int FileLine) const
{
	ISStringMap::const_iterator Iterator = SqlQueryes.find(QueryName);
	if (Iterator == SqlQueryes.end())
	{
		IS_ASSERT(false, QString("SQL query \"%1\" not found. File: %2; Line: %3").arg(QueryName).arg(SourceFile).arg(FileLine));
	}
	return Iterator->second;
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeAnimations()
{
	QFileInfoList FileInfoList = QDir(":Animation").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		AddAnimations(FileInfo.completeBaseName(), FileInfo.filePath());
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeIcons()
{
	QFileInfoList FileInfoList = QDir(":ImageIcons").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		AddImageIcon(FileInfo.completeBaseName(), FileInfo.filePath());
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializePixmaps()
{
	QFileInfoList FileInfoList = QDir(":Images").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		AddImage(FileInfo.completeBaseName(), FileInfo.filePath());
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeAudios()
{
	QFileInfoList FileInfoList = QDir(":Audio").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		AddAudio(FileInfo.completeBaseName(), FileInfo.filePath());
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeStyleSheets()
{
	QFileInfoList FileInfoList = QDir(":CSS").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		AddStyle(FileInfo.completeBaseName(), FileInfo.filePath());
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::InitializeSqlQueryes()
{
	QFileInfoList FileInfoList = QDir(":SQL").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		AddSQL(FileInfo.completeBaseName(), FileInfo.filePath());
	}
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
void ISBuffer::AddStyle(const QString &FileName, const QString &FilePath)
{
	if (StyleSheets.count(FileName))
	{
		IS_ASSERT(false, "StyleSheet '" + FileName + "' already exist in buffer StyleSheets");
	}

	QFile FileStyle(FilePath);
	if (FileStyle.exists())
	{
		if (FileStyle.open(QIODevice::ReadOnly))
		{
			StyleSheets.emplace(FileName, FileStyle.readAll());
			FileStyle.close();
		}
		else
		{
			IS_ASSERT(false, QString("File %1 style sheet not open. Error: %2").arg(FileName).arg(FileStyle.errorString()));
		}
	}
	else
	{
		IS_ASSERT(false, "File " + FileName + " not exist");
	}
}
//-----------------------------------------------------------------------------
void ISBuffer::AddSQL(const QString &FileName, const QString &FilePath)
{
	if (SqlQueryes.count(FileName))
	{
		IS_ASSERT(false, "SQL query '" + FileName + "' already exist in buffer");
	}

	QFile FileSQL(FilePath);
	if (FileSQL.exists())
	{
		if (FileSQL.open(QIODevice::ReadOnly))
		{
			SqlQueryes.emplace(FileName, FileSQL.readAll());
			FileSQL.close();
		}
		else
		{
			IS_ASSERT(false, QString("File %1 sql query not open. Error: %2").arg(FileName).arg(FileSQL.errorString()));
		}
	}
	else
	{
		IS_ASSERT(false, "File " + FileName + " not exist");
	}
}
//-----------------------------------------------------------------------------
