#pragma once
#ifndef _ISTCPANSWER_H_INCLUDED
#define _ISTCPANSWER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISTcpAnswer
{
public:
    ISTcpAnswer(SOCKET socket_client);
    ~ISTcpAnswer();

    bool IsError() const;
    std::string GetErrorString() const;
    void SetError(const std::string &error_string);
    SOCKET GetSocketClient() const; //�������� ���������� ����������� ������
    std::string ToJson() const;
    
private:
    std::string ErrorString;
    bool Error;
    SOCKET SocketClient; //���������� �����
};
//-----------------------------------------------------------------------------
#endif