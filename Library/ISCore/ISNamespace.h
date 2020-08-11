#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISNamespace
{
public:
    enum FieldType //���� ������ ����
    {
        FT_Unknown, //�����������
        FT_Int, //����� �����
        FT_BigInt, //������� ����� �����
        FT_String, //������
        FT_Text, //������� ������
        FT_UID, //���������� �������������
        FT_Bool, //����������
        FT_Double, //����� � ��������� �������
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
        FT_PhoneBase, //����� ��������
        FT_Phone, //����� ��������
        FT_Password, //������
        FT_Image, //�����������
        FT_Color, //����
        FT_EMail, //����� ����������� �����
        FT_Passport, //�������
        FT_Volume, //���������
        FT_Birthday, //���� ��������
        FT_Seconds, //�������
        FT_CallDetails, //����������� ������
        FT_Month, //�����
        FT_Login, //����� ������������ �������
        FT_Address, //�����
        FT_Url, //������
        FT_File, //����
		FT_Sex, //���
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

    enum NotificationFormType //���� ����� �����������
    {
        NFT_Create, //�������� �������
        NFT_Edit, //�������������� �������
        NFT_CreateCopy, //�������� ����� �������
        NFT_Delete, //�������� �������
        NFT_Recovery, //�������������� �������
    };

    enum SearchOperatorType //���� ���������� ������
    {
        SOT_Unknown, //����������
        SOT_Equally, //�����
        SOT_NotEqually, //�� �����
        SOT_Begins, //����������
        SOT_Ends, //�������������
        SOT_Contains, //��������
        SOT_Null, //�����
        SOT_NotNull, //�� �����
        SOT_More, //������
        SOT_Less, //������
        SOT_MoreOrEqual, //������ ��� �����
        SOT_LessOrEqual, //������ ��� �����
        SOT_Range, //��������
    };

    enum ExportType //���� �������� �������� ������
    {
        ET_Unknown,
        ET_CSV,
        ET_HTML,
        ET_DBF,
        ET_XML,
    };

    enum MessageBoxButton
    {
        MBB_Unknown, //���������� (�� ������������ ������ ����)
    };

    enum DeleteRecoveryObject
    {
        DRO_Delete,
        DRO_Recovery,
    };

    enum ActionType
    {
        AT_Create,
        AT_CreateCopy,
        AT_Edit,
        AT_Delete,
        AT_DeleteCascade,
        AT_Update,
        AT_ShowDeleted,
        AT_Search,
        AT_SearchClear,
        AT_Export,
        AT_Print,
        AT_SystemInfo,
        AT_Favorites,
        AT_Reference,
        AT_NavigationBegin,
        AT_NavigationPrevious,
        AT_NavigationNext,
        AT_NavigationLast,
    };

    enum ActionSpecialType
    {
        AST_SortDefault,
        AST_Note,
        AST_ResizeFromContent,
        AST_ResetWidthColumn,
        AST_CopyRecord,
    };

    enum AttachChatType
    {
        ACT_NoAttach,
        ACT_Image,
        ACT_File,
    };

    enum ReportType
    {
        RT_Unknown,
        RT_Html,
        RT_Word,
    };

    enum QueryModelType
    {
        QMT_Object,
        QMT_List,
    };

    enum PhoneIconType //��� ������
    {
        PIT_Standart, //�����������
        PIT_TRUE, //���������� ����
        PIT_FALSE, //������������ ����
    };

    enum PeriodType //��� ���������� �� �������
    {
        PT_UnknownDate,
        PT_CreationDate,
        PT_UpdationDate,
    };

    enum MoveWidgetDesktop //��� ����������� �������
    {
        MWD_LeftUp, //����� ������� ����
        MWD_LeftBottom, //����� ������ ����
        MWD_RightUp, //������ ������� ����
        MWD_RightBottom, //������ ������ ����
        MWD_Center, //����� ������
    };

    enum TelephonyForm
    {
        TF_Journal,
        TF_Summary,
        TF_Pattern,
        TF_Activity,
    };

    enum DebugMessageType //��� ������������ ���������
    {
        DMT_Unknown, //�����������
        DMT_Debug, //����������
        DMT_Info, //��������������
        DMT_Warning, //��������������
        DMT_Error, //������
    };

    enum RangeDatePeriod //��� ������� ���
    {
        RDP_Unknown, //�� ������
        RDP_Today, //�������
        RDP_Yesterday, //�����
        RDP_CurrentMonth, //������� �����
        RDP_LastMonth, //������� �����
        RDP_CurrentYear, //������� ���
        RDP_LastYear, //������� ���
        RDP_Other, //������
    };

};
//-----------------------------------------------------------------------------
Q_DECLARE_METATYPE(ISNamespace::FieldType);
Q_DECLARE_METATYPE(ISNamespace::ObjectFormType);
Q_DECLARE_METATYPE(ISNamespace::NotificationFormType);
Q_DECLARE_METATYPE(ISNamespace::SearchOperatorType);
Q_DECLARE_METATYPE(ISNamespace::ExportType);
Q_DECLARE_METATYPE(ISNamespace::MessageBoxButton);
Q_DECLARE_METATYPE(ISNamespace::DeleteRecoveryObject);
Q_DECLARE_METATYPE(ISNamespace::ActionType);
Q_DECLARE_METATYPE(ISNamespace::ActionSpecialType);
Q_DECLARE_METATYPE(ISNamespace::AttachChatType);
Q_DECLARE_METATYPE(ISNamespace::ReportType);
Q_DECLARE_METATYPE(ISNamespace::QueryModelType);
Q_DECLARE_METATYPE(ISNamespace::PhoneIconType);
Q_DECLARE_METATYPE(ISNamespace::PeriodType);
Q_DECLARE_METATYPE(ISNamespace::MoveWidgetDesktop);
Q_DECLARE_METATYPE(ISNamespace::TelephonyForm);
Q_DECLARE_METATYPE(ISNamespace::DebugMessageType);
Q_DECLARE_METATYPE(ISNamespace::RangeDatePeriod);
//-----------------------------------------------------------------------------
#endif
