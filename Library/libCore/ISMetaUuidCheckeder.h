#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaUuidCheckeder : public QObject
{
	Q_OBJECT

public:
	ISMetaUuidCheckeder(QObject *parent = 0);
	virtual ~ISMetaUuidCheckeder();

    void Search(const QFileInfo &FileInfo);
	void Search(const QString &Content);

private:
	QStringList StringList;
};
//-----------------------------------------------------------------------------
