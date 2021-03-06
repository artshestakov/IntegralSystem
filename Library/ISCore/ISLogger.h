#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISConstants.h"
#include "ISTypedefs.h"
#include "ISNamespace.h"
#include "ISDateTime.h"
//-----------------------------------------------------------------------------
class ISLogger
{
public:
    static ISLogger& Instance();

    const std::string& GetErrorString() const;
    bool Initialize(bool header_use);
    void Shutdown();
    void Log(ISNamespace::LogMessageType MessageType, const std::string &Component, const char *Format, ...);

private:
    bool CreateLogDirectory(const ISDate &Date);
    std::string GetPathFile(const ISDate &Date) const;
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
    bool HeaderUse;
    std::array<std::string, LOG_ARRAY_SIZE> Array; //������ ��������� (�������)
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
#define ISLOGGER_D(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Debug, COMPONENT, FORMAT, ##__VA_ARGS__) //����������� ����������� ���������
#define ISLOGGER_I(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Info, COMPONENT, FORMAT, ##__VA_ARGS__) //����������� ��������������� ���������
#define ISLOGGER_W(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Warning, COMPONENT, FORMAT, ##__VA_ARGS__) //���������� ���������������� ���������
#define ISLOGGER_E(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Error, COMPONENT, FORMAT, ##__VA_ARGS__) //����������� ��������� �� ������
#define ISLOGGER_C(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Critical, COMPONENT, FORMAT, ##__VA_ARGS__) //����������� ����������� ������
#define ISLOGGER_T(FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Trace, std::string(), FORMAT, ##__VA_ARGS__) //�����������
#define ISLOGGER_A(FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Assert, std::string(), FORMAT, ##__VA_ARGS__) //����������� ��������� �� �������
//-----------------------------------------------------------------------------
#endif
