#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISConstants.h"
#include "ISTypedefs.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLogger
{
public:
	static ISLogger& Instance();

	//�������� �������� ������
	QString GetErrorString() const;

	//���������������� ������
	bool Initialize();

	//��������� �������
	void Shutdown();

	//�������� ��������� � ���-����
	void Log(bool is_format, ISNamespace::LogMessageType message_type, const std::string &component, const QString &string);

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
	ISLogger(const ISLogger&) = delete;
	ISLogger(ISLogger&&) = delete;
	ISLogger& operator=(const ISLogger&) = delete;
	ISLogger& operator=(ISLogger&&) = delete;

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
#define ISLOGGER(MESSAGE) ISLogger::Instance().Log(false, ISNamespace::LogMessageType::Unknown, std::string(), MESSAGE) //����������� ��������� ��� ��������������
#define ISLOGGER_D(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Debug, COMPONENT, MESSAGE) //����������� ����������� ���������
#define ISLOGGER_I(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Info, COMPONENT, MESSAGE) //����������� ��������������� ���������
#define ISLOGGER_W(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Warning, COMPONENT, MESSAGE) //���������� ���������������� ���������
#define ISLOGGER_E(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Error, COMPONENT, MESSAGE) //����������� ��������� �� ������
#define ISLOGGER_C(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Critical, COMPONENT, MESSAGE) //����������� ����������� ������
#define ISLOGGER_T(MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Trace, std::string(), MESSAGE) //�����������
#define ISLOGGER_A(MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Assert, std::string(), MESSAGE) //����������� ��������� �� �������
#define ISLOGGER_Q(MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::QT, std::string(), MESSAGE) //����������� ��������� �� �������
//-----------------------------------------------------------------------------
#endif
