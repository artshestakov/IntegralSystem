#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISNetwork : public QObject
{
	Q_OBJECT

public:
	ISNetwork(QObject *parent = 0);
	virtual ~ISNetwork();

	bool CheckAccessInternet(QString &ErrorString) const; //��������� ������ � ���������
	QString ParseIPAddress(const QString &IPAddress) const; //������� IP-������
	bool DownloadFile(const QString &Url, QByteArray &ByteArray, QString &ErrorString) const; //���������� ����� �� ������
};
//-----------------------------------------------------------------------------
