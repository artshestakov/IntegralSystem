#pragma once
#ifndef _ISCARATSERVICE_H_INCLUDED
#define _ISCARATSERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpServerCarat.h"
//-----------------------------------------------------------------------------
class ISCaratService : public QObject
{
	Q_OBJECT

public:
	ISCaratService(QObject *parent = 0);
	virtual ~ISCaratService();

	bool StartService(); //Запуск сервера

private:
	ISTcpServerCarat *TcpServer;
};
//-----------------------------------------------------------------------------
#endif
