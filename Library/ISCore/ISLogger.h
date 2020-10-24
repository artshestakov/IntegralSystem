#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLogger
{
public:
	enum MessageType //���� ���������
	{
		MT_Null, //������ ���������
		MT_Lite, //���������� ��������� (��� ����, ������ � ��������� � ����)
		MT_Debug, //�������
		MT_Info, //����������
		MT_Warning, //��������������
        MT_Error, //������
		MT_Trace, //�����������
        MT_Assert //������
	};

public:

	//�������� ������ �� ������ �������
	static ISLogger& Instance();

	//�������� �������� ������
	QString GetErrorString() const;

	//���������������� ������
	bool Initialize();

	//��������� �������
	void Shutdown();

	//�������� ��������� � ���-����
	void Log(MessageType message_type, const QString &component, const QString &string);

private:

	//������� ����������
	bool CreateLogDirectory(const QDate &Date);

	//�������� ���� � �������� �����
	QString GetPathFile(const QDate &Date) const;

	//����� ������ � �������
	void OutputToConsole(const QString &String);

	//���������� ������� ���������
	void Worker();

private:
	ISLogger();
	~ISLogger();
	ISLogger(ISLogger const &) {};
	ISLogger& operator=(ISLogger const&) { return *this; };

private:
	QString ErrorString; //�������� ������
	std::array<QString, LOGGER_ARRAY_SIZE> Array; //������ ��������� (�������)
	size_t LastIndex; //��������� ������� � �������
	bool IsRunning; //���� ������
	bool IsFinished; //���� ���������
	std::ofstream File; //������� ���-����
	QString PathLogsDir; //������� ���� � �������� ����� � ������
    int CurrentDay; //������� ����
    int CurrentMonth; //������� �����
    int CurrentYear; //������� ���

	//����������� ������ ��� �������������
#ifdef WIN32
    CRITICAL_SECTION CriticalSection;
#else
    pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#define ISLOGGER_N() ISLogger::Instance().Log(ISLogger::MT_Null, QString(), QString()) //����������� ������ ������
#define ISLOGGER_L(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Lite, QString(), MESSAGE) //����������� ����������� ���������
#define ISLOGGER_D(COMPONENT, MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Debug, COMPONENT, MESSAGE) //����������� ����������� ���������
#define ISLOGGER_I(COMPONENT, MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Info, COMPONENT, MESSAGE) //����������� ��������������� ���������
#define ISLOGGER_W(COMPONENT, MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Warning, COMPONENT, MESSAGE) //���������� ���������������� ���������
#define ISLOGGER_E(COMPONENT, MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Error, COMPONENT, MESSAGE) //����������� ��������� �� ������
#define ISLOGGER_T(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Trace, QString(), MESSAGE) //�����������
#define ISLOGGER_A(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Assert, QString(), MESSAGE) //����������� ��������� �� �������
//-----------------------------------------------------------------------------
#endif
