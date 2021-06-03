#pragma once
//-----------------------------------------------------------------------------
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISObjects
{
public:
    static ISObjects& Instance();

    void Initialize(const QString &ConfigurationName);
    ISObjectInterface* GetInterface(); //Получить указатель на ядро конфигурации

private:
    ISObjects();
    ~ISObjects();
    ISObjects(const ISObjects&) = delete;
    ISObjects(ISObjects&&) = delete;
    ISObjects& operator=(const ISObjects&) = delete;
    ISObjects& operator=(ISObjects&&) = delete;

private:
    ISObjectInterface *ObjectInterface;
};
//-----------------------------------------------------------------------------
