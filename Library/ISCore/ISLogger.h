#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISConstants.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLogger
{
public:
	enum MessageType //���� ���������
	{
		MT_Unknown, //����������� ���������
		MT_Debug, //�������
		MT_Info, //����������
		MT_Warning, //��������������
        MT_Error, //������
		MT_Critical, //����������� ������
		MT_Trace, //�����������
        MT_Assert //������
	};

public:

	//�������� ������ �� ������ �������
	static ISLogger& Instance();

	//�������� �������� ������
	QString GetErrorString() const;

	//�������� ����� � ������
	QString GetPathLogsDir() const;

	//���������������� ������
	bool Initialize();

	//��������� �������
	void Shutdown();

	//�������� ��������� � ���-����
	void Log(bool is_format, MessageType message_type, const std::string &component, const QString &string);

private:

	//������� ����������
	bool CreateLogDirectory(const QDate &Date);

	//�������� ���� � �������� �����
	QString GetPathFile(const QDate &Date) const;

	//���������� ������� ���������
	void Worker();

private:
	ISLogger();
	~ISLogger();
	ISLogger(ISLogger const &) {};
	ISLogger& operator=(ISLogger const&) { return *this; };

private:
	QString ErrorString; //�������� ������
	std::array<std::string, LOGGER_ARRAY_SIZE> Array; //������ ��������� (�������)
	size_t LastIndex; //��������� ������� � �������
	bool IsRunning; //���� ������
	bool IsFinished; //���� ���������
	QFile File; //������� ���-����
	QString PathLogsDir; //������� ���� � �������� ����� � ������
	int CurrentDay; //������� ����
	int CurrentMonth; //������� �����
	int CurrentYear; //������� ���

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#define ISLOGGER(MESSAGE) ISLogger::Instance().Log(false, ISLogger::MT_Unknown, std::string(), MESSAGE) //����������� ��������� ��� ��������������
#define ISLOGGER_D(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Debug, COMPONENT, MESSAGE) //����������� ����������� ���������
#define ISLOGGER_I(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Info, COMPONENT, MESSAGE) //����������� ��������������� ���������
#define ISLOGGER_W(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Warning, COMPONENT, MESSAGE) //���������� ���������������� ���������
#define ISLOGGER_E(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Error, COMPONENT, MESSAGE) //����������� ��������� �� ������
#define ISLOGGER_C(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Critical, COMPONENT, MESSAGE) //����������� ����������� ������
#define ISLOGGER_T(MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Trace, std::string(), MESSAGE) //�����������
#define ISLOGGER_A(MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Assert, std::string(), MESSAGE) //����������� ��������� �� �������
//-----------------------------------------------------------------------------
#endif
