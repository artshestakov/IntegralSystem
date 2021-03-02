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

    enum class ObjectFormType //���� ����� �������
    {
        New, //����� ������
        Copy, //����� �������
        Edit, //��������� �������
    };

    enum class SelectListMode //����� ������ ����� ������
    {
        Single, //����� ����� ������
        Multi, //����� ���������� �������
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

    enum class DebugMessageType //��� ������������ ���������
    {
        Unknown, //�����������
        Debug, //����������
        Info, //��������������
        Warning, //��������������
        Error, //������
    };

    enum class LogMessageType //���� ��������� �������
    {
		Unknown, //����������� ���������
		Debug, //�������
		Info, //����������
		Warning, //��������������
		Error, //������
		Critical, //����������� ������
		Trace, //�����������
		Assert, //������
		QT //��������� �� Qt
    };

	enum class VariantType //���� ������
	{
		Invalid, //���������� ��������
		Bool, //���������� ���
		Short, //�������� ��� (16 ���)
		Int, //�������� ��� (32 ����)
		Int64, //�������� ��� (64 ����)
		UInt, //����������� �������� ���
		Double, //������� ���
		Float, //������� ���
		Char, //���������� ���
		String, //��������� ���
		Uuid //���������� �������������
	};
};
//-----------------------------------------------------------------------------
#endif
