#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
namespace ISNamespace
{
    enum class ConfigurationType //���� ������������
    {
        Unknown,
        Empty,
        OilSphere
    };

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

    enum class ObjectFormType //���� ����� �������
    {
        New, //����� ������
        Copy, //����� �������
        Edit, //��������� �������
    };

    enum class SearchType //���� ���������� ������
    {
        Unknown, //����������
        Equally, //�����
        NotEqually, //�� �����
        Begins, //����������
        Ends, //�������������
        Contains, //��������
        More, //������
        Less, //������
        MoreOrEqual, //������ ��� �����
        LessOrEqual, //������ ��� �����
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
        EditDate, //�� ���� ���������
        Size, //�� �������
        Name //�� �����
    };

    enum class ExportType //���� �������� �������� ������
    {
        Unknown,
        CSV,
        HTML,
        XML,
        JSON,
    };

    enum class ActionType
    {
        Create,
        CreateCopy,
        Edit,
        Delete,
        Update,
        Search,
        SearchClear,
        Export,
        Favorites,
        Reference,
    };

    enum class ActionSpecialType
    {
        RecordInfo,
        Favorite,
        Note,
    };

    enum class MoveWidgetDesktop //��� ����������� �������
    {
        LeftUp, //����� ������� ����
        LeftBottom, //����� ������ ����
        RightUp, //������ ������� ����
        RightBottom, //������ ������ ����
        Center, //����� ������
    };

    enum class SelectListMode //����� ������ ����� ������
    {
        Single, //����� ����� ������
        Multi, //����� ���������� �������
    };
};
//-----------------------------------------------------------------------------
#endif
