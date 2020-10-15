#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpSocket.h"
//-----------------------------------------------------------------------------
struct ISTcpMessage
{
	ISNamespace::ApiMessageType Type; //��� ���������
	QVariantMap Parameters; //���������
	ISTcpSocket *TcpSocket; //��������� �� �������
};
//-----------------------------------------------------------------------------
#endif
