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
		MT_Trace, //�����������
		MT_Info, //����������
		MT_Warning, //��������������
        MT_Error, //������
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
	void Log(MessageType type_message, const QString &string_message, const char *source_name, int source_line);

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

#ifdef WIN32
    CRITICAL_SECTION CriticalSection; //����������� ������ ��� �������������
#else
    pthread_mutex_t CriticalSection; //����������� ������ ��� �������������
#endif
};
//-----------------------------------------------------------------------------
#define ISLOGGER_N() ISLogger::Instance().Log(ISLogger::MT_Null, QString(), __FILE__, __LINE__) //����������� ������ ������
#define ISLOGGER_L(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Lite, MESSAGE, __FILE__, __LINE__) //����������� ����������� ���������
#define ISLOGGER_D(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Debug, MESSAGE, __FILE__, __LINE__) //����������� ����������� ���������
#define ISLOGGER_T(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Trace, MESSAGE, __FILE__, __LINE__) //�����������
#define ISLOGGER_I(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Info, MESSAGE, __FILE__, __LINE__) //����������� ��������������� ���������
#define ISLOGGER_W(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Warning, MESSAGE, __FILE__, __LINE__) //���������� ���������������� ���������
#define ISLOGGER_E(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Error, MESSAGE, __FILE__, __LINE__) //����������� ��������� �� ������
#define ISLOGGER_A(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Assert, MESSAGE, __FILE__, __LINE__) //����������� ��������� �� �������
//-----------------------------------------------------------------------------
#endif
