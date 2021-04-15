#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
namespace ISNamespace
{
    enum class FieldType //���� ������ ����
    {
        Unknown, //�����������
        ID, //������������� ������
        Int, //����� �����
        IntArray, //������ ����� �����
        BigInt, //������� ����� �����
        String, //������
        Text, //������� ������
        UID, //���������� �������������
        Bool, //����������
        Double, //����� � ��������� �������
        Money, //������
        ByteArray, //������ ������
        Date, //����
        Time, //�����
        DateTime, //���� � �����
        Inn, //������������� ����� �����������������
        Kpp, //��� ������� ���������� �� ����
        Ogrn, //�������� ��������������� ��������������� �����
        Okpo, //�������������� ������������� ����������� � �����������
        Bik, //���������� ���������������� �����
        Vin, //����������������� ����� ������������� ��������
        Year, //���
        Phone, //����� ��������
        Password, //������
        Image, //�����������
        Color, //����
        EMail, //����� ����������� �����
        Volume, //���������
        Birthday, //���� ��������
        Seconds, //�������
        Month, //�����
        Url, //������
        File, //����
        TaskImportant, //�������� ������
        ProtocolDT, //���� ������� � ���������
    };

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

    enum class SortingOrder //����������� ����������
    {
        Ascending,
        Descending
    };

    enum class DirFileSorting //��� ���������� ������
    {
        DoNotSort, //�� �����������
        CreationDate //�� ���� ��������
    };
};
//-----------------------------------------------------------------------------
#endif
