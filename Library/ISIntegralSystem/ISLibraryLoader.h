#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISLibraryLoader : public QObject
{
	Q_OBJECT

public:
	ISLibraryLoader();
	virtual ~ISLibraryLoader();

	void Load();

protected:
	void AddDir(const QString &DirPath);

private:
	QStringList Files;
};
//-----------------------------------------------------------------------------
