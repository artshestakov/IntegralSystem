#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "document.h"
//-----------------------------------------------------------------------------
class ISTcpMessage
{
public:
	ISTcpMessage();
	~ISTcpMessage();

	std::string GetErrorString() const;
	void SetErrorString(const std::string &error_string);
	bool IsValid() const;

	std::string Type; //��� ���������
	rapidjson::Document Parameters; //���������
	long long MSecParse; //����� ��������
	size_t Size;
	size_t ChunkCount; //���������� ������
	SOCKET Socket; //���������� �����

private:
	std::string ErrorString;
	bool Valid; //���� ���������� ���������
};
//-----------------------------------------------------------------------------
#endif
