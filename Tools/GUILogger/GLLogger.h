#pragma once
#ifndef _GLLOGGER_H_INCLUDED
#define _GLLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <string>
#include <mutex>
#include <fstream>
#include <array>
//-----------------------------------------------------------------------------
#define LOGGER_TIMEOUT 2000
#define ARRAY_MAX_SIZE 10000000
//-----------------------------------------------------------------------------
class GLLogger
{
public:
	enum MessageType
	{
		MT_Info,
		MT_Debug,
		MT_Warning,
		MT_Error
	};

public:
	static GLLogger& Instance(); //Получить ссылку на объект логгера

	std::string GetErrorString() const; //Получить описание ошибки
	bool Initialize(); //Инициализировать логгер

	void Append(MessageType Type, const std::string &String, const char *SourceName, int Line); //Добавить сообщение в лог

private:
	void Worker(); //Обработчик очереди сообщений
	bool CreateDir(); //Создание необходимых директорий
	void UpdateFilePath(); //Обновить путь к файлу

private:
	GLLogger();
	~GLLogger();
	GLLogger(GLLogger const &) {};
	GLLogger& operator=(GLLogger const&) { return *this; };

private:
	std::string ErrorString;
	std::mutex Mutex;
	std::array<std::string, ARRAY_MAX_SIZE> Array;
	size_t LastPosition;
	bool Running;
	std::string PathDirectory;
	std::string PathLogs;
	std::string PathFile;
	std::ofstream File;
	size_t CurrentYear;
	size_t CurrentMonth;
	size_t CurrentDay;
};
//-----------------------------------------------------------------------------
#define LG_INFO(MESSAGE) GLLogger::Instance().Append(GLLogger::MT_Info, MESSAGE, __FILE__, __LINE__)
#define LG_DEBUG(MESSAGE) GLLogger::Instance().Append(GLLogger::MT_Debug, MESSAGE, __FILE__, __LINE__)
#define LG_WARNING(MESSAGE) GLLogger::Instance().Append(GLLogger::MT_Warning, MESSAGE, __FILE__, __LINE__)
#define LG_ERROR(MESSAGE) GLLogger::Instance().Append(GLLogger::MT_Error, MESSAGE, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
