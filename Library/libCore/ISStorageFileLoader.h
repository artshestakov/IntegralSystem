#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISStorageFileLoader : public QObject
{
	Q_OBJECT

signals:
	void ChangeProgressFileMaximum(int Maximum);
	void LoadingFile();

public:
	ISStorageFileLoader(const QString &file_path, const QString &note = QString(), QObject *parent = 0);
	virtual ~ISStorageFileLoader();

	QString GetErrorString() const;
	int GetStorageFileID() const;

	bool Load();

private:
	QString ErrorString;
	QString FilePath;
	QString Note;
	int StorageFileID;
};
//-----------------------------------------------------------------------------
