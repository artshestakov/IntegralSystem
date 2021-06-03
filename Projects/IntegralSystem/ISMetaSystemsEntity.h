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

    std::vector<ISMetaSystem*> GetSystems(); //Получить список всех систем
    ISMetaSystem* GetSystem(const QString &SystemUID); //Получить систему по её идентификатору
    ISMetaSubSystem* GetSubSystem(const QString &SubSystemUID); //Получить подсистему по её идентификатору

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
