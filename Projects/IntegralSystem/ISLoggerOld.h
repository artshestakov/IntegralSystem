#pragma once
#ifndef _ISLOGGEROLD_H_INCLUDED
#define _ISLOGGEROLD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISConstantsOld.h"
#include "ISTypedefsOld.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISLoggerOld
{
public:
	static ISLoggerOld& Instance();

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
    ISLoggerOld();
	~ISLoggerOld();
    ISLoggerOld(const ISLoggerOld&) = delete;
    ISLoggerOld(ISLoggerOld&&) = delete;
    ISLoggerOld& operator=(const ISLoggerOld&) = delete;
    ISLoggerOld& operator=(ISLoggerOld&&) = delete;

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
#define ISLOGGER(MESSAGE) ISLoggerOld::Instance().Log(false, ISNamespace::LogMessageType::Unknown, std::string(), MESSAGE) //����������� ��������� ��� ��������������
#define ISLOGGER_D(COMPONENT, MESSAGE) ISLoggerOld::Instance().Log(true, ISNamespace::LogMessageType::Debug, COMPONENT, MESSAGE) //����������� ����������� ���������
#define ISLOGGER_I(COMPONENT, MESSAGE) ISLoggerOld::Instance().Log(true, ISNamespace::LogMessageType::Info, COMPONENT, MESSAGE) //����������� ��������������� ���������
#define ISLOGGER_W(COMPONENT, MESSAGE) ISLoggerOld::Instance().Log(true, ISNamespace::LogMessageType::Warning, COMPONENT, MESSAGE) //���������� ���������������� ���������
#define ISLOGGER_E(COMPONENT, MESSAGE) ISLoggerOld::Instance().Log(true, ISNamespace::LogMessageType::Error, COMPONENT, MESSAGE) //����������� ��������� �� ������
#define ISLOGGER_C(COMPONENT, MESSAGE) ISLoggerOld::Instance().Log(true, ISNamespace::LogMessageType::Critical, COMPONENT, MESSAGE) //����������� ����������� ������
#define ISLOGGER_T(MESSAGE) ISLoggerOld::Instance().Log(true, ISNamespace::LogMessageType::Trace, std::string(), MESSAGE) //�����������
#define ISLOGGER_A(MESSAGE) ISLoggerOld::Instance().Log(true, ISNamespace::LogMessageType::Assert, std::string(), MESSAGE) //����������� ��������� �� �������
#define ISLOGGER_Q(MESSAGE) ISLoggerOld::Instance().Log(true, ISNamespace::LogMessageType::QT, std::string(), MESSAGE) //����������� ��������� �� �������
//-----------------------------------------------------------------------------
#endif
