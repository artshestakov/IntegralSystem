#pragma once
#ifndef _ASLOGGER_H_INCLUDED
#define _ASLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <string>
#include <mutex>
#include <fstream>
#include <array>
#include <thread>
//-----------------------------------------------------------------------------
#define ARRAY_SIZE 20000
//-----------------------------------------------------------------------------
class ASLogger
{
public:
	enum MessageType //���� ���������
	{
		MT_Debug, //�������
		MT_Info, //����������
		MT_Warning, //��������������
		MT_Error //������
	};

public:
	static ASLogger& Instance(); //�������� ������ �� ������ �������

	std::string GetErrorString() const; //�������� �������� ������
	bool Initialize(const std::string &Prefix = std::string()); //���������������� ������
	void Shutdown(); //��������� �������

	void Log(MessageType message_type, const std::string &message_string, const char *SourceName, int Line);

private:
	bool CreateLogDirectory(); //�������� ����������
	void Worker(); //���������� ������� ���������

private:
	bool CheckExistDir(const std::string &PathDir); //�������� ������������� ����������

private:
	ASLogger();
	~ASLogger();
	ASLogger(ASLogger const &) {};
	ASLogger& operator=(ASLogger const&) { return *this; };

private:
	std::string ErrorString; //�������� ������
	std::mutex Mutex; //������� ��� �������
	std::array<std::string, ARRAY_SIZE> Array; //������ ���������
	bool Running; //���� ������ �������

	std::ofstream File; //������� ���-����
	std::string PathDirLogs; //���� � �������� ����� � ������
	std::string PathDirCurrent; //������� ���� � �������� ����� � ������
	std::string PathFile; //���� � �������� ���-�����

	std::string FilePrefix; //������� ����� �����
};
//-----------------------------------------------------------------------------
#endif
