#pragma once
#ifndef _ISFAIL2BAN_H_INCLUDED
#define _ISFAIL2BAN_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISFail2Ban : public QObject
{
public:
	static ISFail2Ban& Instance();

	void Start(); //������ ������
	bool IsLock(const QString &IPAddress); //�������� ���������� ������
	QDateTime GetUnlockDateTime(const QString &IPAddress); //�������� ���� � ����� ������������� ������
	bool Add(const QString &IPAddress); //�������� ����� � ������ ����������
	void Remove(const QString &IPAddress); //������� ����� �� ������

private:
	ISFail2Ban();
	~ISFail2Ban();
    ISFail2Ban(ISFail2Ban const &) = delete;
	ISFail2Ban(ISFail2Ban &&) = delete;
	ISFail2Ban& operator=(const ISFail2Ban &) = delete;
	ISFail2Ban& operator=(ISFail2Ban &&) = delete;

private:
	void Timeout();

private:
	ISCriticalSection CriticalSection;
	std::vector<ISFailAuthInfo> Vector;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
#endif
