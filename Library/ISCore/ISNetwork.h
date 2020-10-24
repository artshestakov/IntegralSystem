#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISNetwork : public QObject
{
	Q_OBJECT

public:
	ISNetwork(QObject *parent = 0);
	virtual ~ISNetwork();

	QString GetErrorString() const; //Получить текстовое описание последней ошибки

	bool DownloadFile(const QString &Url, QByteArray &ByteArray); //Скачивание файла по ссылке

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
