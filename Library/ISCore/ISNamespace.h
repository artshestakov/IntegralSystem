#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
namespace ISNamespace
{
    enum FieldType //���� ������ ����
    {
        FT_Unknown, //�����������
		FT_ID, //������������� ������
        FT_Int, //����� �����
		FT_IntArray, //������ ����� �����
        FT_BigInt, //������� ����� �����
        FT_String, //������
        FT_Text, //������� ������
        FT_UID, //���������� �������������
        FT_Bool, //����������
        FT_Double, //����� � ��������� �������
		FT_Money, //������
        FT_ByteArray, //������ ������
        FT_Date, //����
        FT_Time, //�����
        FT_DateTime, //���� � �����
        FT_Inn, //������������� ����� �����������������
        FT_Kpp, //��� ������� ���������� �� ����
        FT_Ogrn, //�������� ��������������� ��������������� �����
        FT_Okpo, //�������������� ������������� ����������� � �����������
        FT_Bik, //���������� ���������������� �����
        FT_Vin, //����������������� ����� ������������� ��������
        FT_Year, //���
        FT_Phone, //����� ��������
        FT_Password, //������
        FT_Image, //�����������
        FT_Color, //����
        FT_EMail, //����� ����������� �����
        FT_Volume, //���������
        FT_Birthday, //���� ��������
        FT_Seconds, //�������
        FT_Month, //�����
        FT_Url, //������
        FT_File, //����
		FT_TaskImportant, //�������� ������
		FT_ProtocolDT, //�������� ������
    };

    enum ObjectFormType //���� ����� �������
    {
        OFT_New, //����� ������
        OFT_Copy, //����� �������
        OFT_Edit, //��������� �������
    };

    enum SelectListMode //����� ������ ����� ������
    {
        SLM_Single, //����� ����� ������
        SLM_Multi, //����� ���������� �������
    };

    enum SearchOperatorType //���� ���������� ������
    {
        SOT_Unknown, //����������
        SOT_Equally, //�����
        SOT_NotEqually, //�� �����
        SOT_Begins, //����������
        SOT_Ends, //�������������
        SOT_Contains, //��������
        SOT_More, //������
        SOT_Less, //������
        SOT_MoreOrEqual, //������ ��� �����
        SOT_LessOrEqual, //������ ��� �����
    };

    enum ExportType //���� �������� �������� ������
    {
        ET_Unknown,
        ET_CSV,
        ET_HTML,
        ET_XML,
		ET_JSON,
    };

    enum ActionType
    {
        AT_Create,
        AT_CreateCopy,
        AT_Edit,
        AT_Delete,
        AT_Update,
        AT_Search,
        AT_SearchClear,
        AT_Export,
        AT_Favorites,
        AT_Reference,
		AT_PageBegin,
		AT_PagePrevious,
		AT_PageSelect,
		AT_PageNext,
		AT_PageEnd,
        AT_NavigationBegin,
        AT_NavigationPrevious,
        AT_NavigationNext,
        AT_NavigationLast,
    };

    enum ActionSpecialType
    {
		AST_RecordInfo,
        AST_Note,
    };

    enum ReportType
    {
        RT_Unknown,
        RT_Html,
    };

    enum MoveWidgetDesktop //��� ����������� �������
    {
        MWD_LeftUp, //����� ������� ����
        MWD_LeftBottom, //����� ������ ����
        MWD_RightUp, //������ ������� ����
        MWD_RightBottom, //������ ������ ����
        MWD_Center, //����� ������
    };

    enum DebugMessageType //��� ������������ ���������
    {
        DMT_Unknown, //�����������
        DMT_Debug, //����������
        DMT_Info, //��������������
        DMT_Warning, //��������������
        DMT_Error, //������
    };

	enum ApiMessageType //��� API-���������
	{
		AMT_Unknown, //�����������
		AMT_Auth, //�����������
		AMT_Sleep, //��������
		AMT_GetMetaData, //�������� ����-������
		AMT_GetLastClient, //�������� ��������� ������ �������
		AMT_UserPasswordExist, //��������� ������������� ������
		AMT_UserPasswordCreate, //�������� ������ ������������
		AMT_UserPasswordEdit, //��������� ������ ������������
		AMT_UserPasswordReset, //����� ������ ��������������
		AMT_UserSettingsReset, //����� ���������������� ��������
		AMT_GetRecordCall, //�������� ������ ���������
		AMT_GetClients, //�������� ������ ������������ ��������
		AMT_RecordAdd, //���������� ������
		AMT_RecordEdit, //��������� ������
		AMT_RecordDelete, //�������� ������
		AMT_RecordGet, //�������� ������
		AMT_RecordGetInfo, //�������� ���������� � ������
		AMT_DiscussionAdd, //�������� ����������
		AMT_DiscussionEdit, //�������� ����������
		AMT_DiscussionCopy, //����������� ����������
		AMT_GetTableData, //�������� ������ �������
		AMT_GetTableQuery, //�������� ������ �� �������
		AMT_NoteRecordGet, //�������� ���������� ������
		AMT_NoteRecordSet, //�������� ���������� ������
		AMT_FileStorageAdd, //�������� ���� � ���������
		AMT_FileStorageCopy, //����������� ���� � ���������
		AMT_FileStorageGet, //�������� ���� �� ���������
		AMT_SearchTaskText, //����� ����� �� ������
		AMT_SearchTaskID, //����� ����� �� ������
		AMT_SearchFullText, //�������������� �����
		AMT_GetCalendarEvents, //�������� ������� ���������
		AMT_CalendarDelete, //�������� ������� ���������
		AMT_GetInternalLists, //�������� ������ ���������� ������������
		AMT_SaveMetaData, //���������� ����-������
		AMT_GetGroupRights, //�������� ����� ������
		AMT_GroupRightSubSystemAdd, //�������� ������ ����� �� ����������
		AMT_GroupRightSubSystemDelete, //������� � ������ ����� �� ����������
		AMT_GroupRightTableAdd, //�������� ������ ����� �� �������
		AMT_GroupRightTableDelete, //������� � ������ ����� �� �������
		AMT_GroupRightSpecialAdd, //�������� ����. ����� ������
		AMT_GroupRightSpecialDelete, //������� ����. ����� � ������
		AMT_GetRecordValue, //�������� �������� ���� ������
		AMT_RecordFavoriteAdd, //�������� ������ � ���������
		AMT_RecordFavoriteDelete, //������� ������ �� ����������
		AMT_GetFavoriteNames, //�������� ����� ��������� �������
		AMT_FavoritesDelete, //������� ��� ��������� ������ ������������
		AMT_LogGetStructure, //�������� ��������� ���-������
		AMT_LogGetContent, //�������� ���������� ���-�����
		AMT_CalendarClose, //�������� ������� ���������
		AMT_GetHistoryList, //�������� ������ �������� �������
		AMT_TaskCommentAdd, //�������� ����������� � ������
		AMT_GetForeignList, //�������� ������ �������� �� �������� �����
		AMT_GetServerInfo, //�������� ���������� � �������
		AMT_OrganizationFromINN, //����� ����������� �� ���
		AMT_PeriodContains, //�������� ������� �������� �������
		AMT_GetStockList, //�������� ������ �������
		AMT_StatementAdd, //�������� �������� � ��������� ���
		AMT_GetGasStation, //��������� �� ���������
		AMT_GetDebtImplementation, //�������� �������� � ��������
		AMT_GetDebtCounterparty, //�������� ����� �����������
	};
};
//-----------------------------------------------------------------------------
Q_DECLARE_METATYPE(ISNamespace::FieldType);
Q_DECLARE_METATYPE(ISNamespace::ObjectFormType);
Q_DECLARE_METATYPE(ISNamespace::SearchOperatorType);
Q_DECLARE_METATYPE(ISNamespace::ExportType);
Q_DECLARE_METATYPE(ISNamespace::ActionType);
Q_DECLARE_METATYPE(ISNamespace::ActionSpecialType);
Q_DECLARE_METATYPE(ISNamespace::ReportType);
Q_DECLARE_METATYPE(ISNamespace::MoveWidgetDesktop);
Q_DECLARE_METATYPE(ISNamespace::DebugMessageType);
Q_DECLARE_METATYPE(ISNamespace::ApiMessageType);
//-----------------------------------------------------------------------------
#endif
