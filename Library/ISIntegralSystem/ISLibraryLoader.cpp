#include "StdAfx.h"
#include "ISLibraryLoader.h"
#include "EXDefines.h"
#include "ISSystem.h"
#include "ISDebug.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
ISLibraryLoader::ISLibraryLoader() : QObject()
{
	AddDir(APPLICATION_DIR_PATH + "/audio");
	AddDir(APPLICATION_DIR_PATH + "/bearer");
	AddDir(APPLICATION_DIR_PATH + "/iconengines");
	AddDir(APPLICATION_DIR_PATH + "/imageformats");
	AddDir(APPLICATION_DIR_PATH + "/mediaservice");
	AddDir(APPLICATION_DIR_PATH + "/platforms");
	AddDir(APPLICATION_DIR_PATH + "/playlistformats");
	AddDir(APPLICATION_DIR_PATH + "/printsupport");
	AddDir(APPLICATION_DIR_PATH + "/sqldrivers");

	QString FilterString;

	if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
	{
		FilterString = QString("Qt5*");
	}
	else if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Linux)
	{
		FilterString = QString("libQt5*");
	}

	FilterString += "." + ISSystem::GetLibraryExtension();

	QFileInfoList FileInfoList = QDir(APPLICATION_DIR_PATH).entryInfoList(QStringList() << FilterString, QDir::Files);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		Files.append(FileInfo.absoluteFilePath());
	}
}
//-----------------------------------------------------------------------------
ISLibraryLoader::~ISLibraryLoader()
{

}
//-----------------------------------------------------------------------------
void ISLibraryLoader::AddDir(const QString &DirPath)
{
    QFileInfoList FileInfoList = QDir(DirPath).entryInfoList(QStringList() << QString("*.%1").arg(ISSystem::GetLibraryExtension()), QDir::Files);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		Files.append(FileInfo.absoluteFilePath());
	}
}
//-----------------------------------------------------------------------------
void ISLibraryLoader::Load()
{
	ISCountingTime CountingTimeFiles;

	for (const QString &FilePath : Files)
	{
		ISCountingTime CountingTimeFile;
		QString FileName = ISSystem::GetFileName(FilePath);

		if (QLibrary::isLibrary(FilePath))
		{
			QLibrary Library(FilePath, this);
			if (Library.isLoaded())
			{
				ISDebug::ShowInfoString(QString("Library %1 already loaded").arg(FileName));
			}
			else
			{
				if (Library.load())
				{
					ISDebug::ShowInfoString(QString("Library %1 loaded %2 msec").arg(FileName).arg(CountingTimeFile.GetElapsed()));
				}
				else
				{
					ISDebug::ShowInfoString(QString("Library %1 not loaded").arg(FileName));
				}
			}
		}
		else
		{
			ISDebug::ShowInfoString(QString("Library %1 not library").arg(FileName));
		}
	}

	ISDebug::ShowInfoString(QString("Loaded %1 Qt-libs: %2 msec").arg(Files.count()).arg(CountingTimeFiles.GetElapsed()));
}
//-----------------------------------------------------------------------------
