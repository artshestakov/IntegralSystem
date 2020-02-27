//-----------------------------------------------------------------------------
#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
class ISLogger
{
public:
	enum MessageType //��� ���������
	{
		MT_Info,
		MT_Debug,
		MT_Warning,
		MT_Error
	};

public:
	static ISLogger& Instance(); //�������� ������ �� ������ �������

	std::string GetErrorString() const; //�������� �������� ������
	bool Initialize(); //���������������� ������

	void Log(MessageType Type, const std::string &String, const char *SourceName, int Line); //�������� ��������� � ���

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
	std::mutex Mutex; //�������
	std::array<std::string, ARRAY_MAX_SIZE> Array; //������ ���������
	size_t LastPosition; //�������� �������
	bool Running; //���� ������ �������
	std::string PathDirectory; //���� � ����� � ����������� ������ ����������
	std::string PathLogs; //���� � ����� � ������
	std::string PathFile; //���� � �������� ���-�����
	std::ofstream File; //������� ���-����
	size_t CurrentYear; //������� ���
	size_t CurrentMonth; //������� �����
	size_t CurrentDay; //������� ����
};
//-----------------------------------------------------------------------------
#define LG_INFO(MESSAGE) GLLogger::Instance().Log(GLLogger::MT_Info, MESSAGE, __FILE__, __LINE__) //����������� ��������������� ���������
#define LG_DEBUG(MESSAGE) GLLogger::Instance().Log(GLLogger::MT_Debug, MESSAGE, __FILE__, __LINE__) //����������� ����������� ���������
#define LG_WARNING(MESSAGE) GLLogger::Instance().Log(GLLogger::MT_Warning, MESSAGE, __FILE__, __LINE__) //����������� ��������������
#define LG_ERROR(MESSAGE) GLLogger::Instance().Log(GLLogger::MT_Error, MESSAGE, __FILE__, __LINE__) //����������� ������
//-----------------------------------------------------------------------------
#endif
