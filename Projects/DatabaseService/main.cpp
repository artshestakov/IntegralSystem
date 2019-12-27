#include "StdAfx.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISConfig.h"
#include "ISLocalization.h"
#include "ISDebug.h"
#include "DSWorker.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);

	ISConfig::GetInstance().Initialize(CONFIG_FILE_PATH);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CORE);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_DATABASE_SERVICE);

	QString BackupFolder = CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_FOLDERBACKUP);
	if (!QDir(BackupFolder).exists())
	{
		ISDebug::ShowWarningString(LOCALIZATION("BackupFolderNotExist").arg(BackupFolder));
		return EXIT_CODE_NORMAL;
	}

	QString PostgresFolder = CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_FOLDERPOSTGRESBIN);
	if (!QDir(PostgresFolder).exists())
	{
		ISDebug::ShowWarningString(LOCALIZATION("PostgresFolderNotExist").arg(PostgresFolder));
		return EXIT_CODE_NORMAL;
	}

	DSWorker Worker;
	Worker.ClearBackupFolder();
	Worker.Start();

	return EXIT_CODE_NORMAL;
}
//-----------------------------------------------------------------------------
