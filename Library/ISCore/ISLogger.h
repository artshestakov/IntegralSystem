#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISConstants.h"
#include "ISTypedefs.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLogger
{
public:
	static ISLogger& Instance();

	//Получить описание ошибки
	QString GetErrorString() const;

	//Инициализировать логгер
	bool Initialize();

	//Остановка логгера
	void Shutdown();

	//Добавить сообщение в лог-файл
	void Log(bool is_format, ISNamespace::LogMessageType message_type, const std::string &component, const QString &string);

private:

	//Создать директорию
	bool CreateLogDirectory(const QDate &Date);

	//Получить путь к текущему файлу
	QString GetPathFile(const QDate &Date) const;

	//Обработчик очереди сообщений
	void Worker();

private:
	ISLogger();
	~ISLogger();
	ISLogger(const ISLogger&) = delete;
	ISLogger(ISLogger&&) = delete;
	ISLogger& operator=(const ISLogger&) = delete;
	ISLogger& operator=(ISLogger&&) = delete;

private:
	QString ErrorString; //Описание ошибки
	std::array<std::string, LOGGER_ARRAY_SIZE> Array; //Массив сообщений (очередь)
	size_t LastIndex; //Последняя позиция в очереди
	bool IsRunning; //Флаг работы
	bool IsFinished; //Флаг остановки
	QFile File; //Текущий лог-файл
	QString PathLogsDir; //Текущий путь к конечной папке с логами
	int CurrentDay; //Текущий день
	int CurrentMonth; //Текущий месяц
	int CurrentYear; //Текущий год

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#define ISLOGGER(MESSAGE) ISLogger::Instance().Log(false, ISNamespace::LogMessageType::Unknown, std::string(), MESSAGE) //Логирование сообщения без форматирования
#define ISLOGGER_D(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Debug, COMPONENT, MESSAGE) //Логирование отладочного сообщения
#define ISLOGGER_I(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Info, COMPONENT, MESSAGE) //Логирование информационного сообщения
#define ISLOGGER_W(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Warning, COMPONENT, MESSAGE) //Логировние предупреждающего сообщения
#define ISLOGGER_E(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Error, COMPONENT, MESSAGE) //Логирование сообщения об ошибке
#define ISLOGGER_C(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Critical, COMPONENT, MESSAGE) //Логирование критической ошибки
#define ISLOGGER_T(MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Trace, std::string(), MESSAGE) //Трассировка
#define ISLOGGER_A(MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::Assert, std::string(), MESSAGE) //Логирование сообщения об ассерте
#define ISLOGGER_Q(MESSAGE) ISLogger::Instance().Log(true, ISNamespace::LogMessageType::QT, std::string(), MESSAGE) //Логирование сообщения об ассерте
//-----------------------------------------------------------------------------
#endif
