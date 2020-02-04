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

	static ISLogger& GetInstance(); //�������� ������ �������
	QString GetLogFileName() const; //�������� ���� � ���-�����

	void AddMessage(const QString &Message); //�������� ��������� � ���-����
	void AddMessageFunc(const QString &Function, const QString &Message); //�������� ��������� � ���-���� �� �������

protected:
	void Timeout();
	void AddData(); //���������� ������ � ����

	void UpdateLogFileName(); //�������� ���� � ���-�����

private:
	ISLogger();

	QFutureWatcher<void> *FutureWatcher;
	QQueue<QString> Queue;
	QString LogPath;
};
//-----------------------------------------------------------------------------
#define IS_LOGGER(Message) ISLogger::GetInstance().AddMessage(Message) //�������� ��������� � ���-����
#define IS_LOGGER_FUNC(Message) ISLogger::GetInstance().AddMessageFunc(Q_FUNC_INFO, Message) //�������� ��������� � ���-���� �� �������
//-----------------------------------------------------------------------------
#endif
