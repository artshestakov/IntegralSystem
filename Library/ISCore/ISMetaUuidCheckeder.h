#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaUuidCheckeder : public QObject
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
