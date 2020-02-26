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

	static void ShowString(const QString &Message = QString()); //����� �������� ���������
	static void ShowDebugString(const QString &Message); //����� ����������� ���������
	static void ShowInfoString(const QString &Message); //����� ��������������� ���������
	static void ShowWarningString(const QString &Message); //����� ��������������
	static void ShowErrorString(const QString &Message); //����� ������������ ���������

private:
	void OutputString(ISNamespace::DebugMessageType MessageType, const QString &Message, bool AddInLog = true); //����� ���������
	
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
