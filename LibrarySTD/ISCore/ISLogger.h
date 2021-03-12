#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISConstants.h"
#include "ISTypedefs.h"
#include "ISNamespace.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLogger
{
public:
	static ISLogger& Instance();
	static std::string GetClassName(const std::string &FunctionName);

	std::string GetErrorString() const;
	bool Initialize();
	void Shutdown();
	void Log(bool is_format, ISNamespace::LogMessageType message_type, const std::string &component, const std::string &message);

private:
	bool CreateLogDirectory(const ISDateTime &DT);
	std::string GetPathFile(const ISDateTime &DT) const;
	void Worker();

private:
	ISLogger();
	~ISLogger();
	ISLogger(const ISLogger&) = delete;
	ISLogger(ISLogger&&) = delete;
	ISLogger& operator=(const ISLogger&) = delete;
	ISLogger& operator=(ISLogger&&) = delete;

private:
	std::string ErrorString; //�������� ������
	std::array<std::string, LOGGER_ARRAY_SIZE> Array; //������ ��������� (�������)
	size_t LastIndex; //��������� ������� � �������
	bool IsRunning; //���� ������
	bool IsFinished; //���� ���������
	std::ofstream File; //������� ���-����
	std::string PathLogsDir; //������� ���� � �������� ����� � ������
	unsigned short CurrentDay; //������� ����
	unsigned short CurrentMonth; //������� �����
	unsigned short CurrentYear; //������� ���
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
#ifdef WIN32
#define __CLASS__ ISLogger::GetClassName(__FUNCTION__)
#else
#define __CLASS__ ISLogger::GetClassName(__PRETTY_FUNCTION__)
#endif
//-----------------------------------------------------------------------------
#endif
