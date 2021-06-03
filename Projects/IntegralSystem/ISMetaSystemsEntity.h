#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISMetaSystemsEntity
{
public:
    static ISMetaSystemsEntity& Instance();

    void Initialize(const QVariantList &VariantList);

    std::vector<ISMetaSystem*> GetSystems(); //�������� ������ ���� ������
    ISMetaSystem* GetSystem(const QString &SystemUID); //�������� ������� �� � ��������������
    ISMetaSubSystem* GetSubSystem(const QString &SubSystemUID); //�������� ���������� �� � ��������������

private:
    ISMetaSystem* CheckExistSystem(const QString &SystemUID);

private:
    ISMetaSystemsEntity();
    ~ISMetaSystemsEntity();
    ISMetaSystemsEntity(const ISMetaSystemsEntity&) = delete;
    ISMetaSystemsEntity(ISMetaSystemsEntity&&) = delete;
    ISMetaSystemsEntity& operator=(const ISMetaSystemsEntity&) = delete;
    ISMetaSystemsEntity& operator=(ISMetaSystemsEntity&&) = delete;

private:
    std::vector<ISMetaSystem*> Systems;
};
//-----------------------------------------------------------------------------
