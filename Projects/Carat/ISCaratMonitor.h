#pragma once
#ifndef _ISCARATMONITOR_H_INCLUDED
#define _ISCARATMONITOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratMonitor
{
public:
	static ISCaratMonitor& Instance();

public:
	void IncomingQuerySize(quint64 QuerySize); //Регистрация размера входящего запроса
	void AnswerQuerySize(quint64 AnswerSize); //Регистрация размера ответа на запрос

private:
	ISCaratMonitor();
	~ISCaratMonitor();
	ISCaratMonitor(ISCaratMonitor const &) {};
	ISCaratMonitor& operator=(ISCaratMonitor const&) { return *this; };

private:
	quint64 _IncomingQuerySize; //Размер входящего траффика
	quint64 _AnswerQuerySize; //Размер исходящего траффика
};
//-----------------------------------------------------------------------------
#endif
