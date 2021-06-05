#pragma once
#ifndef _ISFAIL2BAN_H_INCLUDED
#define _ISFAIL2BAN_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISFail2Ban
{
public:
    static ISFail2Ban& Instance();

    bool Exist(const std::string &IPAddress, size_t &Index); //�������� ������� ������ � ������
    bool Exist(const std::string &IPAddress); //�������� ������� ������ � ������
    void FailAuth(const std::string &IPAddress, size_t &AttemptLeft); //������������� ��������� ����
    bool IsEmptyAttempts(const std::string &IPAddress); //���������, �������� �� ��� ������� ��� �����
    bool IsEmptyAttempts(const std::string &IPAddress, uint64_t &DateTimeUnlock); //���������, �������� �� ��� ������� ��� �����
    void Remove(const std::string &IPAddress); //������� ����� �� ������

private:
    ISFail2Ban();
    ~ISFail2Ban();
    ISFail2Ban(const ISFail2Ban&) = delete;
    ISFail2Ban(ISFail2Ban&&) = delete;
    ISFail2Ban& operator=(const ISFail2Ban&) = delete;
    ISFail2Ban& operator=(ISFail2Ban&&) = delete;

private:
    std::vector<ISLockedIP> Vector;
};
//-----------------------------------------------------------------------------
#endif
