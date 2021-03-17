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
	static std::string GetClassName(char *FunctionName);

	std::string GetErrorString() const;
	bool Initialize();
	void Shutdown();
	void Log(ISNamespace::LogMessageType MessageType, const std::string &Component, const char *Format, ...);

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
	std::string ErrorString; //Описание ошибки
	std::array<std::string, LOG_ARRAY_SIZE> Array; //Массив сообщений (очередь)
	size_t LastIndex; //Последняя позиция в очереди
	bool IsRunning; //Флаг работы
	bool IsFinished; //Флаг остановки
	std::ofstream File; //Текущий лог-файл
	std::string PathLogsDir; //Текущий путь к конечной папке с логами
	unsigned short CurrentDay; //Текущий день
	unsigned short CurrentMonth; //Текущий месяц
	unsigned short CurrentYear; //Текущий год
	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#define ISLOGGER_D(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Debug, COMPONENT, FORMAT, __VA_ARGS__) //Логирование отладочного сообщения
#define ISLOGGER_I(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Info, COMPONENT, FORMAT, __VA_ARGS__) //Логирование информационного сообщения
#define ISLOGGER_W(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Warning, COMPONENT, FORMAT, __VA_ARGS__) //Логировние предупреждающего сообщения
#define ISLOGGER_E(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Error, COMPONENT, FORMAT, __VA_ARGS__) //Логирование сообщения об ошибке
#define ISLOGGER_C(COMPONENT, FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Critical, COMPONENT, FORMAT, __VA_ARGS__) //Логирование критической ошибки
#define ISLOGGER_T(FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Trace, std::string(), FORMAT, __VA_ARGS__) //Трассировка
#define ISLOGGER_A(FORMAT, ...) ISLogger::Instance().Log(ISNamespace::LogMessageType::Assert, std::string(), FORMAT, __VA_ARGS__) //Логирование сообщения об ассерте
//-----------------------------------------------------------------------------
#endif
