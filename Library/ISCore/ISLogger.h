#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLogger
{
public:
	enum MessageType //Типы сообщений
	{
		MT_Null, //Пустое сообщение
		MT_Lite, //Упрощенное сообщение (без даты, потока и источника в коде)
		MT_Debug, //Отладка
		MT_Info, //Информация
		MT_Warning, //Предупреждение
        MT_Error, //Ошибка
		MT_Trace, //Трассировка
        MT_Assert //Ассерт
	};

public:

	//Получить ссылку на объект логгера
	static ISLogger& Instance();

	//Получить описание ошибки
	QString GetErrorString() const;

	//Инициализировать логгер
	bool Initialize();

	//Остановка логгера
	void Shutdown();

	//Добавить сообщение в лог-файл
	void Log(MessageType message_type, const QString &component, const QString &string);

private:

	//Создать директорию
	bool CreateLogDirectory(const QDate &Date);

	//Получить путь к текущему файлу
	QString GetPathFile(const QDate &Date) const;

	//Вывод строки в консоль
	void OutputToConsole(const QString &String);

	//Обработчик очереди сообщений
	void Worker();

private:
	ISLogger();
	~ISLogger();
	ISLogger(ISLogger const &) {};
	ISLogger& operator=(ISLogger const&) { return *this; };

private:
	QString ErrorString; //Описание ошибки
	std::array<QString, LOGGER_ARRAY_SIZE> Array; //Массив сообщений (очередь)
	size_t LastIndex; //Последняя позиция в очереди
	bool IsRunning; //Флаг работы
	bool IsFinished; //Флаг остановки
	std::ofstream File; //Текущий лог-файл
	QString PathLogsDir; //Текущий путь к конечной папке с логами
    int CurrentDay; //Текущий день
    int CurrentMonth; //Текущий месяц
    int CurrentYear; //Текущий год

	//Критическая секция для синхронизации
#ifdef WIN32
    CRITICAL_SECTION CriticalSection;
#else
    pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#define ISLOGGER_N() ISLogger::Instance().Log(ISLogger::MT_Null, QString(), QString()) //Логирование пустой строки
#define ISLOGGER_L(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Lite, QString(), MESSAGE) //Логирование упрощенного сообщения
#define ISLOGGER_D(COMPONENT, MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Debug, COMPONENT, MESSAGE) //Логирование отладочного сообщения
#define ISLOGGER_I(COMPONENT, MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Info, COMPONENT, MESSAGE) //Логирование информационного сообщения
#define ISLOGGER_W(COMPONENT, MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Warning, COMPONENT, MESSAGE) //Логировние предупреждающего сообщения
#define ISLOGGER_E(COMPONENT, MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Error, COMPONENT, MESSAGE) //Логирование сообщения об ошибке
#define ISLOGGER_T(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Trace, QString(), MESSAGE) //Трассировка
#define ISLOGGER_A(MESSAGE) ISLogger::Instance().Log(ISLogger::MT_Assert, QString(), MESSAGE) //Логирование сообщения об ассерте
//-----------------------------------------------------------------------------
#endif
