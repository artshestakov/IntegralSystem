//-----------------------------------------------------------------------------
#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISNamespace.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
#define ISLOGGER_EMPTY() ISLogger::Instance().Log(ISNamespace::DMT_Unknown, QString(), __FILE__, __LINE__) //����������� ������������ ��������� (� �������)
#define ISLOGGER_UNKNOWN(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Unknown, MESSAGE, __FILE__, __LINE__) //����������� ������������ ���������
#define ISLOGGER_INFO(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Info, MESSAGE, __FILE__, __LINE__) //����������� ��������������� ���������
#define ISLOGGER_DEBUG(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Debug, MESSAGE, __FILE__, __LINE__) //����������� ����������� ���������
#define ISLOGGER_WARNING(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Warning, MESSAGE, __FILE__, __LINE__) //����������� ��������������
#define ISLOGGER_ERROR(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Error, MESSAGE, __FILE__, __LINE__) //����������� ������
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISLogger
{
public:
	static ISLogger& Instance(); //�������� ������ �� ������ �������

	QString GetErrorString() const; //�������� �������� ������
	bool Initialize(bool OutPrintf, bool OutFile, const std::string &file_prefix = std::string()); //���������������� ������

	void Log(ISNamespace::DebugMessageType Type, const QString &String, const char *SourceName, int Line); //�������� ��������� � ���
	void Shutdown();

private:
	void Worker(); //���������� ������� ���������
	bool CreateDir(); //�������� ����������� ����������
	void UpdateFilePath(); //�������� ���� � �����

private:
	ISLogger();
	~ISLogger();
	ISLogger(ISLogger const &) {};
	ISLogger& operator=(ISLogger const&) { return *this; };

private:
	std::string ErrorString; //�������� ������
	std::mutex Mutex; //������� ��� �������
	std::array<std::string, ARRAY_MAX_SIZE> Array; //������ ���������
	size_t LastPosition; //�������� �������
	bool Running; //���� ������ �������
	bool Finished; //���� ��������� �������
	char Year[5];
	
	std::ofstream File; //������� ���-����
	std::string PathDirectory; //���� � ����� � ����������� ������ ����������
	std::string PathLogs; //���� � ����� � ������
	std::string PathFile; //���� � �������� ���-�����

	size_t CurrentYear; //������� ���
	size_t CurrentMonth; //������� �����
	size_t CurrentDay; //������� ����

	bool EnableOutPrintf; //���� ������ � �������
	bool EnableOutFile; //���� ������ � ����
	std::string FilePrefix; //������� ����� �����
};
//-----------------------------------------------------------------------------
#endif