#pragma once
//-----------------------------------------------------------------------------
#include "iscarat_global.h"
#include "ISCaratCoreApplication.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISCARAT_EXPORT ISCoreTelephony : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreTelephony(int &argc, char **argv);
	virtual ~ISCoreTelephony();

	bool Invoke() override;

protected:
	void Timeout();
	void HandlingCDR(int ID); //��������� ������ CDR

	QVariant GetDialEnd(int Duration, int BillSec) const; //�������� ������������ ��������
	QVariant GetDirection(const ISUuid &DirectionUID) const; //�������� ������������� �����������
	QVariant GetDialStatus(int ID, const QString &DialStatus) const; //�������� ������������� ������� ������
	QString GetPattern(int ID, const QString &String) const; //�������� ���������� �����
	QVariant GetUser(int ID, const QVariant &Pattern) const; //�������� ������������� ������������
	bool CheckExistCall(const QString &UniqueID) const; //�������� ������������� ������ � UniqueID � ����

private:
	QTimer *Timer;
	int AsteriskDigitNumbers;
};
//-----------------------------------------------------------------------------
