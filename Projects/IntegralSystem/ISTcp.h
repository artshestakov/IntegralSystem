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
	static unsigned int GetQuerySizeFromBuffer(QByteArray &ByteArray, bool &Ok); //�������� ������ ������� �� ��� �����������
	static void WaitForBytesWritten(QTcpSocket *TcpSocket); //�������� ������ ������ � �����
	static void WaitForDisconnected(QTcpSocket *TcpSocket); //�������� ���������� �� �����
};
//-----------------------------------------------------------------------------
#endif
