#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISConstants.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLogger
{
public:
	enum MessageType //Типы сообщений
	{
		MT_Unknown, //Неизвестное сообщение
		MT_Debug, //Отладка
		MT_Info, //Информация
		MT_Warning, //Предупреждение
        MT_Error, //Ошибка
		MT_Critical, //Критическая ошибка
		MT_Trace, //Трассировка
        MT_Assert //Ассерт
	};

public:

	//Получить ссылку на объект логгера
	static ISLogger& Instance();

	//Получить описание ошибки
	QString GetErrorString() const;

	//Получить папку с логами
	QString GetPathLogsDir() const;

	//Инициализировать логгер
	bool Initialize();

	//Остановка логгера
	void Shutdown();

	//Добавить сообщение в лог-файл
	void Log(bool is_format, MessageType message_type, const std::string &component, const QString &string);

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
	ISLogger(ISLogger const &) {};
	ISLogger& operator=(ISLogger const&) { return *this; };

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
#define ISLOGGER(MESSAGE) ISLogger::Instance().Log(false, ISLogger::MT_Unknown, std::string(), MESSAGE) //Логирование сообщения без форматирования
#define ISLOGGER_D(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Debug, COMPONENT, MESSAGE) //Логирование отладочного сообщения
#define ISLOGGER_I(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Info, COMPONENT, MESSAGE) //Логирование информационного сообщения
#define ISLOGGER_W(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Warning, COMPONENT, MESSAGE) //Логировние предупреждающего сообщения
#define ISLOGGER_E(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Error, COMPONENT, MESSAGE) //Логирование сообщения об ошибке
#define ISLOGGER_C(COMPONENT, MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Critical, COMPONENT, MESSAGE) //Логирование критической ошибки
#define ISLOGGER_T(MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Trace, std::string(), MESSAGE) //Трассировка
#define ISLOGGER_A(MESSAGE) ISLogger::Instance().Log(true, ISLogger::MT_Assert, std::string(), MESSAGE) //Логирование сообщения об ассерте
//-----------------------------------------------------------------------------
#endif
