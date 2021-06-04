#pragma once
#ifndef _ISTCP_H_INCLUDED
#define _ISTCP_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISTcp
{
public:
	static unsigned int GetQuerySizeFromBuffer(QByteArray &ByteArray, bool &Ok); //Получить размер запроса из его содержимого
	static void WaitForBytesWritten(QTcpSocket *TcpSocket); //Ожидание записи данных в сокет
	static void WaitForDisconnected(QTcpSocket *TcpSocket); //Ожидание отключения от хоста
};
//-----------------------------------------------------------------------------
#endif
