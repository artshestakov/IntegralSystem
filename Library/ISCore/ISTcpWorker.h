#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpWorker : public QObject
{
	Q_OBJECT

public:
	ISTcpWorker();
	virtual ~ISTcpWorker();

	bool GetRunning(); //�������� ���� ���������
	void SetMessage(ISTcpMessage *TcpMessage); //���������� ��������� �� ���������
	void Run(); //������ �������

private:
	void Finish(); //����������� � ���������� ������ �������
	bool CheckNullField(const QString &FieldName, const QVariantMap &VariantMap); //�������� ������� ����

private:
	bool Auth(ISTcpMessage *TcpMessage);

private:
	QString ErrorString; //��������� �������� ������ �������
	bool IsRunning; //���� ��������� �������
	ISTcpMessage *CurrentMessage; //��������� �� ������� ���������

	//����������� ������ ��� �������������
#ifdef WIN32
	CRITICAL_SECTION CriticalSection;
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
