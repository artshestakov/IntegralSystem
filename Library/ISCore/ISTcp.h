#pragma once
#ifndef _ISTCP_H_INCLUDED
#define _ISTCP_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcp
{
public:
	static unsigned int GetQuerySizeFromBuffer(QByteArray &ByteArray, bool &Ok); //�������� ������ ������� �� ��� �����������
	static void WaitForBytesWritten(QTcpSocket *TcpSocket); //�������� ������ ������ � �����
};
//-----------------------------------------------------------------------------
#endif
