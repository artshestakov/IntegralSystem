#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "rapidjson/document.h"
#include "ISTcpClient.h"
//-----------------------------------------------------------------------------
class ISTcpMessage
{
public:
    ISTcpMessage();
    ~ISTcpMessage();

    const std::string& GetErrorString() const;
    void SetErrorString(const std::string &error_string);
    bool IsValid() const;

    std::string Type; //��� ���������
    rapidjson::Document Parameters; //���������
    uint64_t MSecParse; //����� ��������
    size_t Size;
    size_t ChunkCount; //���������� ������
    ISTcpClient *TcpClient; //������, �� �������� ������ ��� ���������

private:
    std::string ErrorString;
    bool Valid; //���� ���������� ���������
};
//-----------------------------------------------------------------------------
#endif
