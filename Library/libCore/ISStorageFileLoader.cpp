#include "ISStorageFileLoader.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISSystem.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QI_FILE = PREPARE_QUERY("INSERT INTO _storagefiles(sgfs_owneruser, sgfs_name, sgfs_expansion, sgfs_size, sgfs_note) "
									   "VALUES(:OwnerUser, :Name, :Expansion, :Size, :Note) "
									   "RETURNING sgfs_id");
//-----------------------------------------------------------------------------
static QString QI_FILE_DATA = PREPARE_QUERY("INSERT INTO _storagefilesdata(sgfd_storagefile, sgfd_data) "
											"VALUES(:StorageFile, :Data)");
//-----------------------------------------------------------------------------
ISStorageFileLoader::ISStorageFileLoader(const QString &file_path, const QString &note, QObject *parent) : QObject(parent)
{
	FilePath = file_path;
	Note = note;
	StorageFileID = -1;
}
//-----------------------------------------------------------------------------
ISStorageFileLoader::~ISStorageFileLoader()
{

}
//-----------------------------------------------------------------------------
QString ISStorageFileLoader::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
int ISStorageFileLoader::GetStorageFileID() const
{
	return StorageFileID;
}
//-----------------------------------------------------------------------------
bool ISStorageFileLoader::Load()
{
	QFile File(FilePath);
	if (!File.open(QIODevice::ReadOnly))
	{
		ErrorString = File.errorString();
		return false;
	}

	QFileInfo FileInfo(FilePath);
	ISDatabase::GetInstance().GetDefaultDB().transaction(); //Открытие транзакции

	try
	{
		ISQuery qInsertFile(QI_FILE);
		qInsertFile.BindValue(":OwnerUser", ISMetaUser::GetInstance().GetData()->ID);
		qInsertFile.BindValue(":Name", FileInfo.baseName());
		qInsertFile.BindValue(":Expansion", FileInfo.completeSuffix());
		qInsertFile.BindValue(":Size", ISSystem::FileSizeFromString(ISSystem::GetFileSize(File.fileName())));
		
		if (Note.length())
		{
			qInsertFile.BindValue(":Note", Note);
		}
		else
		{
			qInsertFile.BindValue(":Note", QVariant());
		}

		qInsertFile.ExecuteFirst();
		StorageFileID = qInsertFile.ReadColumn("sgfs_id").toInt();

		emit ChangeProgressFileMaximum(File.size() / (8 * 1024));

		while (!File.atEnd())
		{
			QByteArray ByteArray = File.read(8 * 1024);

			ISQuery qInsertData(QI_FILE_DATA);
			qInsertData.BindValue(":StorageFile", StorageFileID);
			qInsertData.BindValue(":Data", qCompress(ByteArray, 9));
			qInsertData.Execute();

			emit LoadingFile();
		}
	}
	catch (ISQueryException &QueryException)
	{
		ISDatabase::GetInstance().GetDefaultDB().rollback(); //Откат транзакции
		return false;
	}

	ISDatabase::GetInstance().GetDefaultDB().commit(); //Коммит

	File.close();
	return true;
}
//-----------------------------------------------------------------------------
