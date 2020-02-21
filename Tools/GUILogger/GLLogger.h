#pragma once
#ifndef _GLLOGGER_H_INCLUDED
#define _GLLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <chrono>
#include <list>
#include <windows.h>
#include <sysinfoapi.h>
//-----------------------------------------------------------------------------
#define LOGGER_TIMEOUT 2
#define ARRAY_MAX_SIZE 1000
//-----------------------------------------------------------------------------
class GLLogger
{
public:
	enum MessageType
	{
		MT_Unknown,
		MT_Info,
		MT_Debug,
		MT_Warning,
		MT_Error
	};

public:
	static GLLogger& Instance(); //Получить ссылку на объект логгера

	std::string GetErrorString() const; //Получить описание ошибки
	bool Initialize(); //Инициализировать логгер

	void Append(MessageType Type, const std::string &String, const char *SourceName, int Line); //Добавить сообщение в очередь

private:
	void Worker(); //Обработчик очереди сообщений
	bool CreateDirs(); //Создание необходимых директорий

private:
	GLLogger();
	~GLLogger();
	GLLogger(GLLogger const &) {};
	GLLogger& operator=(GLLogger const&) { return *this; };

private:
	std::string ErrorString;
	std::mutex Mutex;
	char **Array;
	size_t LastPosition;
	bool Running;
	FILE *File;
};
//-----------------------------------------------------------------------------
#define LG_INFO(MESSAGE) GLLogger::Instance().Append(GLLogger::MT_Info, MESSAGE, __FILE__, __LINE__)
#define LG_DEBUG(MESSAGE) GLLogger::Instance().Append(GLLogger::MT_Debug, MESSAGE, __FILE__, __LINE__)
#define LG_WARNING(MESSAGE) GLLogger::Instance().Append(GLLogger::MT_Warning, MESSAGE, __FILE__, __LINE__)
#define LG_ERROR(MESSAGE) GLLogger::Instance().Append(GLLogger::MT_Error, MESSAGE, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
