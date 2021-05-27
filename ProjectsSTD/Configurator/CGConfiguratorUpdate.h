#pragma once
#ifndef _CGCONFIGURATORUPDATE_H_INCLUDED
#define _CGCONFIGURATORUPDATE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorUpdate : public CGConfiguratorBase
{
public:
    CGConfiguratorUpdate();
    virtual ~CGConfiguratorUpdate();

    bool database(); //Обновление базы данных
    bool functions(); //Обновление функций
    bool tables(); //Обновление таблиц
    bool comment(); //Комментирование таблиц
    bool indexes(); //Обновление индексов
    bool foreigns(); //Обновление внешних ключей
    bool resources(); //Обновление ресурсов
    bool databasesettings(); //Обновление настроек БД
    //bool protocol(); //Очистка протокола
};
//-----------------------------------------------------------------------------
#endif
