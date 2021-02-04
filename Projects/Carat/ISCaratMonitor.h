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
	ISCaratMonitor(const ISCaratMonitor&) = delete;
	ISCaratMonitor(ISCaratMonitor&&) = delete;
	ISCaratMonitor& operator=(const ISCaratMonitor&) = delete;
	ISCaratMonitor& operator=(ISCaratMonitor&&) = delete;

private:
	quint64 _IncomingQuerySize; //Размер входящего траффика
	quint64 _AnswerQuerySize; //Размер исходящего траффика
};
//-----------------------------------------------------------------------------
#endif
