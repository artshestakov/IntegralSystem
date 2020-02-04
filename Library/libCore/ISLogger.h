#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISLogger : public QObject
{
	Q_OBJECT

public:
	ISLogger(const ISLogger &) = delete;
	ISLogger(ISLogger &&) = delete;
	ISLogger &operator=(const ISLogger &) = delete;
	ISLogger &operator=(ISLogger &&) = delete;
	~ISLogger();

	static ISLogger& GetInstance(); //Получить объект логгера
	QString GetLogFileName() const; //Получить путь к лог-файлу

	void AddMessage(const QString &Message); //Добавить сообщение в лог-файл
	void AddMessageFunc(const QString &Function, const QString &Message); //Добавить сообщение в лог-файл из функции

protected:
	void Timeout();
	void AddData(); //Добавление данных в файл

	void UpdateLogFileName(); //Обновить путь к лог-файлу

private:
	ISLogger();

	QFutureWatcher<void> *FutureWatcher;
	QQueue<QString> Queue;
	QString LogPath;
};
//-----------------------------------------------------------------------------
#define IS_LOGGER(Message) ISLogger::GetInstance().AddMessage(Message) //Добавить сообщение в лог-файл
#define IS_LOGGER_FUNC(Message) ISLogger::GetInstance().AddMessageFunc(Q_FUNC_INFO, Message) //Добавить сообщение в лог-файл из функции
//-----------------------------------------------------------------------------
#endif
