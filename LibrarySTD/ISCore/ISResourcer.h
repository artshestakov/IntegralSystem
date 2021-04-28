#pragma once
#ifndef _ISRESOURCER_H_INCLUDED
#define _ISRESOURCER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISResourcer
{
public:
    static ISResourcer& Instance();

    const std::string& GetErrorString() const; //�������� ��������� �������� ������
    bool LoadFile(const std::string &FilePath); //��������� ����
    void Free(); //���������� ������ � �������
    
    const char* GetFile(const std::string &FileName); //�������� ����
    const char* GetFile(const std::string &FileName, unsigned long &FileSize); //�������� ����

private:
    bool ParseFile(); //������� �����
    bool ParseHeader(const std::string &StringHeader, std::string &FileName, unsigned long &FileSize); //������� ��������� �����

private:
    ISResourcer();
    ~ISResourcer();
    ISResourcer(const ISResourcer&) = delete;
    ISResourcer(ISResourcer&&) = delete;
    ISResourcer& operator=(const ISResourcer&) = delete;
    ISResourcer& operator=(ISResourcer&&) = delete;

private:
    std::string ErrorString;
    char *Data;
    long SizeData;
    std::vector<ISResourceFile> Files;
    size_t FileCount;
};
//-----------------------------------------------------------------------------
#endif
