#pragma once
#ifndef _ISSTRUCTSGUI_H_INCLUDED
#define _ISSTRUCTSGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISAlgorithm.h"
#include "ISTypedefsGUI.h"
//-----------------------------------------------------------------------------
struct ISCurrentUserInfo
{
    ISCurrentUserInfo() : System(false), ID(0), GroupID(0), GroupFullAccess(false) { }

    bool System; //���������
    unsigned int ID; //������������� ������������
    QString FIO; //���
    QString Login; //�����
    QString Password; //������ (��������)

    unsigned int GroupID; //������������� ������ � ������� ������� ������������
    bool GroupFullAccess; //������ ������
};
//-----------------------------------------------------------------------------
struct ISMetaSetting
{
    ISMetaSetting() : Type(ISNamespace::FieldType::Unknown) { }

    QString UID;
    QString Name;
    ISNamespace::FieldType Type;
    QString WidgetEditName;
    QString LocalName;
    QString Hint;
    QVariant Default;
    QVariant Value;
};
//-----------------------------------------------------------------------------
struct ISMetaSettingsGroup
{
    ISMetaSettingsGroup() : System(true) { }
    ~ISMetaSettingsGroup()
    {
        while (!Settings.empty())
        {
            delete ISAlgorithm::VectorTakeBack(Settings);
        }
    }

    QString Name;
    QString UID;
    QString LocalName;
    QString IconName;
    bool System;
    QString Hint;
    std::vector<ISMetaSetting*> Settings;
};
//-----------------------------------------------------------------------------
struct ISMetaParagraph
{
    QString UID;
    QString Name;
    QString LocalName;
    QString ToolTip;
    QString Icon;
    QString ClassName;
};
//-----------------------------------------------------------------------------
struct ISMetaSubSystem
{
    QString UID; //������������� ����������
    QString LocalName; //��������� ��� ����������
    QString IconName; //��� ������ ����������
    QString TableName; //��� ������� ����������
    QString ClassName; //��� ������ ����������
    QString Hint; //���������

    QString SystemUID; //������������� ������� � ������� ��������� ��� ����������
    QString SystemLocalName; //��� ������������ �������
};
//-----------------------------------------------------------------------------
struct ISMetaSystem
{
    ISMetaSystem() : IsSystem(false) { }

    ~ISMetaSystem()
    {
        while (!SubSystems.empty())
        {
            delete ISAlgorithm::VectorTakeBack(SubSystems);
        }
    }

    bool IsSystem; //��������� ������� (���� ��� - ������ ����������������)
    QString UID; //�������������
    QString LocalName; //��� �������
    QString IconName; //��� ������
    QString Hint; //����������� ���������
    std::vector<ISMetaSubSystem*> SubSystems; //������ ���������
};
//-----------------------------------------------------------------------------
struct ISMessageBoxButton
{
    ISMessageBoxButton(int id, const QString& text, bool _default = false, const QIcon& icon = QIcon())
        : ID(id),
        Text(text),
        Default(_default),
        Icon(icon)
    {

    }

    int ID; //������������� ������
    QString Text; //�������
    bool Default; //�� ������ ����� ���������� �����
    QIcon Icon; //������
};
//-----------------------------------------------------------------------------
struct ISModelField
{
    ISModelField() : Index(0), Type(ISNamespace::FieldType::Unknown), IsForeign(false), IsSystem(false) { }

    size_t Index; //������
    QString Name; //���
    QString LocalName; //��������� ���
    ISNamespace::FieldType Type; //���
    bool IsForeign; //���� �������� �����
    bool IsSystem; //���� ���������� ����
};
//-----------------------------------------------------------------------------
struct ISModelRecord
{
    ISModelRecord() : ID(0) { }

    unsigned int ID; //������������� ������
    ISVectorVariantQt Values; //������ ��������
};
//-----------------------------------------------------------------------------
#endif
