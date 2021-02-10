#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
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

    enum class ApiMessageType //��� API-���������
	{
		Unknown, //�����������
		Auth, //�����������
		Sleep, //��������
		GetMetaData, //�������� ����-������
		GetLastClient, //�������� ��������� ������ �������
		UserPasswordExist, //��������� ������������� ������
		UserPasswordCreate, //�������� ������ ������������
		UserPasswordEdit, //��������� ������ ������������
		UserPasswordReset, //����� ������ ��������������
		UserSettingsReset, //����� ���������������� ��������
		UserDeviceAdd, //�������� �������� � ������� ������ ������������
		UserDeviceDelete, //������� �������� �� ������� ������ ������������
		GetRecordCall, //�������� ������ ���������
		GetClients, //�������� ������ ������������ ��������
		RecordAdd, //���������� ������
		RecordEdit, //��������� ������
		RecordDelete, //�������� ������
		RecordGet, //�������� ������
		RecordGetInfo, //�������� ���������� � ������
		DiscussionAdd, //�������� ����������
		DiscussionEdit, //�������� ����������
		DiscussionCopy, //����������� ����������
		GetTableData, //�������� ������ �������
		GetTableQuery, //�������� ������ �� �������
		NoteRecordGet, //�������� ���������� ������
		NoteRecordSet, //�������� ���������� ������
		FileStorageAdd, //�������� ���� � ���������
		FileStorageCopy, //����������� ���� � ���������
		FileStorageGet, //�������� ���� �� ���������
		SearchTaskText, //����� ����� �� ������
		SearchTaskID, //����� ����� �� ������
		SearchFullText, //�������������� �����
		GetCalendarEvents, //�������� ������� ���������
		CalendarDelete, //�������� ������� ���������
		GetInternalLists, //�������� ������ ���������� ������������
		SaveMetaData, //���������� ����-������
		GetGroupRights, //�������� ����� ������
		GroupRightSubSystemAdd, //�������� ������ ����� �� ����������
		GroupRightSubSystemDelete, //������� � ������ ����� �� ����������
		GroupRightTableAdd, //�������� ������ ����� �� �������
		GroupRightTableDelete, //������� � ������ ����� �� �������
		GroupRightSpecialAdd, //�������� ����. ����� ������
		GroupRightSpecialDelete, //������� ����. ����� � ������
		GetRecordValue, //�������� �������� ���� ������
		RecordFavoriteAdd, //�������� ������ � ���������
		RecordFavoriteDelete, //������� ������ �� ����������
		GetFavoriteNames, //�������� ����� ��������� �������
		FavoritesDelete, //������� ��� ��������� ������ ������������
		LogGetStructure, //�������� ��������� ���-������
		LogGetContent, //�������� ���������� ���-�����
		CalendarClose, //�������� ������� ���������
		GetHistoryList, //�������� ������ �������� �������
		TaskCommentAdd, //�������� ����������� � ������
		GetForeignList, //�������� ������ �������� �� �������� �����
		GetServerInfo, //�������� ���������� � �������
		OrganizationFromINN, //����� ����������� �� ���
		GetStatement, //�������� ���������� ��������
		PeriodContains, //�������� ������� �������� �������
		GetStockList, //�������� ������ �������
		StatementAdd, //�������� �������� � ��������� ���
		GetGasStation, //��������� �� ���������
		GetDebtImplementation, //�������� �������� � ��������
		GetDebtCounterparty, //�������� ����� �����������
		GetUserConsumption, //�������� ������� �������������
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
		Assert //������
    };
};
//-----------------------------------------------------------------------------
Q_DECLARE_METATYPE(ISNamespace::FieldType);
Q_DECLARE_METATYPE(ISNamespace::ObjectFormType);
Q_DECLARE_METATYPE(ISNamespace::SearchType);
Q_DECLARE_METATYPE(ISNamespace::ExportType);
Q_DECLARE_METATYPE(ISNamespace::ActionType);
Q_DECLARE_METATYPE(ISNamespace::ActionSpecialType);
Q_DECLARE_METATYPE(ISNamespace::MoveWidgetDesktop);
Q_DECLARE_METATYPE(ISNamespace::DebugMessageType);
Q_DECLARE_METATYPE(ISNamespace::ApiMessageType);
Q_DECLARE_METATYPE(ISNamespace::LogMessageType);
//-----------------------------------------------------------------------------
#endif
