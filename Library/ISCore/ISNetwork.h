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

	QString GetErrorString() const; //�������� ��������� �������� ��������� ������

	bool DownloadFile(const QString &Url, QByteArray &ByteArray); //���������� ����� �� ������

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
