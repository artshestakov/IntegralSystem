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
    InitializeStyleSheets();
}
//-----------------------------------------------------------------------------
QMovie* ISBuffer::GetAnimation(const QString &AnimationName, QObject *parent, const char *SourceFile, int FileLine)
{
    auto It = Animations.find(AnimationName);
    if (It == Animations.end())
    {
        IS_ASSERT(false, QString("Animation \"%1\" not found in buffer animations. File: %2. Line: %3.").arg(AnimationName).arg(SourceFile).arg(FileLine).toStdString());
    }
    return new QMovie(It->second, nullptr, parent);
}
//-----------------------------------------------------------------------------
QIcon ISBuffer::GetIcon(const QString &IconName, const char *SourceFile, int FileLine)
{
    auto It = Icons.find(IconName);
    if (It == Icons.end())
    {
        IS_ASSERT(false, QString("Icon \"%1\" not found in buffer icons. File: %2. Line: %3.").arg(IconName).arg(SourceFile).arg(FileLine).toStdString());
    }
    return It->second;
}
//-----------------------------------------------------------------------------
QPixmap ISBuffer::GetPixmap(const QString &PixmapName, const char *SourceFile, int FileLine)
{
    auto It = Pixmaps.find(PixmapName);
    if (It == Pixmaps.end())
    {
        IS_ASSERT(false, QString("Pixmap \"%1\" not found in buffer pixmaps. File: %2. Line: %3.").arg(PixmapName).arg(SourceFile).arg(FileLine).toStdString());
    }
    return It->second;
}
//-----------------------------------------------------------------------------
QString ISBuffer::GetStyle(const QString &StyleName, const char *SourceFile, int FileLine) const
{
    auto It = StyleSheets.find(StyleName);
    if (It == StyleSheets.end())
    {
        IS_ASSERT(false, QString("StyleSheet \"%1\" not found. File: %2; Line: %3").arg(StyleName).arg(SourceFile).arg(FileLine).toStdString());
    }
    return It->second;
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
void ISBuffer::InitializeStyleSheets()
{
    QFileInfoList FileInfoList = QDir(":CSS").entryInfoList(QDir::NoFilter);
    for (const QFileInfo &FileInfo : FileInfoList)
    {
        AddStyle(FileInfo.completeBaseName(), FileInfo.filePath());
    }
}
//-----------------------------------------------------------------------------
void ISBuffer::AddAnimations(const QString &AnimationName, const QString &AnimationPath)
{
    auto It = Animations.find(AnimationName);
    if (It == Animations.end())
    {
        Animations.emplace(AnimationName, AnimationPath);
    }
    else
    {
        IS_ASSERT(false, QString("Animation \"%1\" already exist in buffer animations").arg(AnimationName).toStdString());
    }
}
//-----------------------------------------------------------------------------
void ISBuffer::AddImageIcon(const QString &IconName, const QString &IconPath)
{
    auto It = Icons.find(IconName);
    if (It == Icons.end())
    {
        Icons.emplace(IconName, QIcon(IconPath));
    }
    else
    {
        IS_ASSERT(false, QString("Icon \"%1\" already exist in buffer icons").arg(IconName).toStdString());
    }
}
//-----------------------------------------------------------------------------
void ISBuffer::AddImage(const QString &PixmapName, const QString &PixmapPath)
{
    auto It = Pixmaps.find(PixmapName);
    if (It == Pixmaps.end())
    {
        Pixmaps.emplace(PixmapName, QPixmap(PixmapPath));
    }
    else
    {
        IS_ASSERT(false, QString("Pixmap \"%1\" already exist in buffer pixmaps").arg(PixmapName).toStdString());
    }
}
//-----------------------------------------------------------------------------
void ISBuffer::AddStyle(const QString &FileName, const QString &FilePath)
{
    auto It = StyleSheets.find(FileName);
    if (It == StyleSheets.end())
    {
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
                IS_ASSERT(false, QString("File %1 style sheet not open. Error: %2").arg(FileName).arg(FileStyle.errorString()).toStdString());
            }
        }
        else
        {
            IS_ASSERT(false, QString("File " + FileName + " not exist").toStdString());
        }
    }
    else
    {
        IS_ASSERT(false, QString("StyleSheet '" + FileName + "' already exist in buffer StyleSheets").toStdString());
    }
}
//-----------------------------------------------------------------------------
