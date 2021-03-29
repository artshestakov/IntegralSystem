#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
namespace ISNamespace
{
    enum class LogMessageType //���� ��������� �������
    {
        Unknown, //����������� ���������
        Assert, //������
        Critical, //����������� ������
        Error, //������
        Warning, //��������������
        Info, //����������
        Debug, //�������
        Trace //�����������
    };
};
//-----------------------------------------------------------------------------
#endif
