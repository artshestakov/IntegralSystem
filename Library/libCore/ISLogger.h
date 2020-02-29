//-----------------------------------------------------------------------------
#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISNamespace.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
#define ISLOGGER_EMPTY() ISLogger::Instance().Log(ISNamespace::DMT_Unknown, QString(), __FILE__, __LINE__) //Логирование неизвестного сообщения (в консоль)
#define ISLOGGER_UNKNOWN(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Unknown, MESSAGE, __FILE__, __LINE__) //Логирование неизвестного сообщения
#define ISLOGGER_INFO(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Info, MESSAGE, __FILE__, __LINE__) //Логирование информационного сообщения
#define ISLOGGER_DEBUG(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Debug, MESSAGE, __FILE__, __LINE__) //Логирование отладочного сообщения
#define ISLOGGER_WARNING(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Warning, MESSAGE, __FILE__, __LINE__) //Логирование предупреждения
#define ISLOGGER_ERROR(MESSAGE) ISLogger::Instance().Log(ISNamespace::DMT_Error, MESSAGE, __FILE__, __LINE__) //Логирование ошибки
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISLogger
{
public:
	static ISLogger& Instance(); //Получить ссылку на объект логгера

	QString GetErrorString() const; //Получить описание ошибки
	bool Initialize(bool OutPrintf, bool OutFile, const std::string &file_prefix = std::string()); //Инициализировать логгер

	void Log(ISNamespace::DebugMessageType Type, const QString &String, const char *SourceName, int Line); //Добавить сообщение в лог
	void Shutdown();

private:
	void Worker(); //Обработчик очереди сообщений
	bool CreateDir(); //Создание необходимых директорий
	void UpdateFilePath(); //Обновить путь к файлу

private:
	ISLogger();
	~ISLogger();
	ISLogger(ISLogger const &) {};
	ISLogger& operator=(ISLogger const&) { return *this; };

private:
	std::string ErrorString; //Описание ошибки
	std::mutex Mutex; //Мьютекс для массива
	std::array<std::string, ARRAY_MAX_SIZE> Array; //Массив сообщений
	size_t LastPosition; //Посденяя позиция
	bool Running; //Флаг работы логгера
	bool Finished; //Флаг остановки логгера
	char Year[5];
	
	std::ofstream File; //Текущий лог-файл
	std::string PathDirectory; //Путь к папке с исполняемым файлом приложения
	std::string PathLogs; //Путь к папке с логами
	std::string PathFile; //Путь к текущему лог-файлу

	size_t CurrentYear; //Текущий год
	size_t CurrentMonth; //Текущий месяц
	size_t CurrentDay; //Текущий день

	bool EnableOutPrintf; //Флаг вывода в консоль
	bool EnableOutFile; //Флаг вывода в файл
	std::string FilePrefix; //Префикс имени файла
};
//-----------------------------------------------------------------------------
#endif