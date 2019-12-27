#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISDebug : public QObject
{
	Q_OBJECT

signals:
	void Output(const QString &String);

public:
	ISDebug(const ISDebug &) = delete;
	ISDebug(ISDebug &&) = delete;
	ISDebug &operator=(const ISDebug &) = delete;
	ISDebug &operator=(ISDebug &&) = delete;
	~ISDebug();

	static ISDebug& GetInstance();

	static void ShowCaratString(const QString &CoreName, const QString &String);
	static void ShowString(const QString &String);
	static void ShowEmptyString(bool AddInLog = true);

	static void ShowDebugString(const QString &String); //Вывод отладочного сообщения
	static void ShowInfoString(const QString &String); //Вывод информационного сообщения
	static void ShowWarningString(const QString &String); //Вывод предупреждения
	static void ShowCriticalString(const QString &String); //Вывод критического сообщения
	static void ShowAssertString(const QString &String); //Вывод сообщения ассерта
	static void ShowExceptionString(const QString &String); //Вывод сообщения исключения

	void OutputString(const QString &String, bool AddInLog = true); //Вывод сообщения
	void EmptyString(bool AddInLog = true); //Добавить пустую строку

	void CaratString(const QString &CoreName, const QString &String); //Вывод сообщения в Карат
	void DebugString(const QString &String); //Вывод отладочного сообщения
	void InfoString(const QString &String); //Вывод информационного сообщения
	void WarningString(const QString &String); //Вывод предупреждения
	void CriticalString(const QString &String); //Вывод критического сообщения
	void AssertString(const QString &String); //Вывод сообщения ассерта
	void ExceptionString(const QString &String); //Вывод сообщения исключения

	void SetAssertMessage(const QString &assert_message);
	QString GetAssertMessage() const;
	QStringList GetDebugMessages() const;

protected:
	QString GetCompleteString(ISNamespace::DebugMessageType MessageType, const QString &String) const; //Сформировать строку
	
private:
	ISDebug();

	QStringList Messages;
	QString AssertMessage;
	QMutex Mutex;
};
//-----------------------------------------------------------------------------
