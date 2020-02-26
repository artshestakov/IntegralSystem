#pragma once
#ifndef _ISDEBUG_H_INCLUDED
#define _ISDEBUG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISDebug
{
public:
	static ISDebug& GetInstance();

	static void ShowString(const QString &Message = QString()); //Вывод обычного сообщения
	static void ShowDebugString(const QString &Message); //Вывод отладочного сообщения
	static void ShowInfoString(const QString &Message); //Вывод информационного сообщения
	static void ShowWarningString(const QString &Message); //Вывод предупреждения
	static void ShowErrorString(const QString &Message); //Вывод критического сообщения

private:
	void OutputString(ISNamespace::DebugMessageType MessageType, const QString &Message, bool AddInLog = true); //Вывод сообщения
	
private:
	ISDebug();
	~ISDebug();
	ISDebug(ISDebug const &) {};
	ISDebug& operator=(ISDebug const&) { return *this; };

private:
	QStringList Messages;
};
//-----------------------------------------------------------------------------
#endif
