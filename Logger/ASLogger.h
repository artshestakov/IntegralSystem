#pragma once
#ifndef _ASLOGGER_H_INCLUDED
#define _ASLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <thread>
#include <windows.h>
#include <minwindef.h>
//-----------------------------------------------------------------------------
#define ARRAY_SIZE 20000
//-----------------------------------------------------------------------------
class ASLogger
{
public:
	enum MessageType //���� ���������
	{
		MT_Lite, //����������
		MT_Debug, //�������
		MT_Info, //����������
		MT_Warning, //��������������
		MT_Error //������
	};

public:

	//�������� ������ �� ������ �������
	static ASLogger& Instance();

	//�������� �������� ������
	std::string GetErrorString() const;

	//���������������� ������
	bool Initialize(const std::string &prefix = std::string());

	//��������� �������
	void Shutdown();

	//�������� ��������� � ���-����
	void Log(MessageType type_message, const std::string &string_message, const char *source_name, int source_line);

private:

	//������� ����������
	bool CreateLogDirectory();

	//�������� ������������� ����������
	bool CheckExistDir(const std::string &path_dir);

	//���������� ������� ���������
	void Worker();

private:
	ASLogger();
	~ASLogger();
	ASLogger(ASLogger const &) {};
	ASLogger& operator=(ASLogger const&) { return *this; };

private:
	std::string ErrorString; //�������� ������
	CRITICAL_SECTION CriticalSection; //����������� ������ ��� �������������
	std::array<std::string, ARRAY_SIZE> Array; //������ ��������� (�������)
	size_t LastIndex; //��������� ������� � �������
	bool IsRunning; //���� ������
	bool IsFinished; //���� ���������

	std::ofstream File; //������� ���-����
	std::string PathDirLogs; //���� � �������� ����� � ������
	std::string PathDirCurrent; //������� ���� � �������� ����� � ������
	std::string PathFile; //���� � �������� ���-�����

	std::string FilePrefix; //������� ����� �����
};
//-----------------------------------------------------------------------------
#define ASLOGGER_L(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Lite, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_D(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Debug, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_I(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Info, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_W(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Warning, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_E(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Error, MESSAGE, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
