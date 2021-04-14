#pragma once
#ifndef _ISDATABASE_H_INCLUDED
#define _ISDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
#include "ISTypedefs.h"
#include "libpq-fe.h"
//-----------------------------------------------------------------------------
class ISDatabase
{
public:
    static ISDatabase& Instance();

    std::string GetErrorString() const; //Получить описание последней ошибки

    PGconn* GetDB(const std::string &ConnectionName); //Получить ссылку на экземпляр БД по имени соединения
    ISConnectOptionDB GetOption(const std::string &ConnectionName); //Получить параметры подключения к БД

    bool Connect(const std::string &ConnectionName, const ISConnectOptionDB &ConnectOptionDB); //Подключение к БД
    bool Connect(const std::string &ConnectionName, const std::string &Host, unsigned short Port, const std::string &Database, const std::string &Login, const std::string &Password); //Подключение к БД
    void Disconnect(const std::string &ConnectionName); //Отключение от БД
    void DisconnectAll(); //Отключение от всех БД

private:
    ISDatabase();
    ~ISDatabase();
    ISDatabase(const ISDatabase&) = delete;
    ISDatabase(ISDatabase&&) = delete;
    ISDatabase& operator=(const ISDatabase&) = delete;
    ISDatabase& operator=(ISDatabase&&) = delete;

private:
    std::string ErrorString;
    std::map<std::string, PGconn*> ConnectionsMap;
    ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
