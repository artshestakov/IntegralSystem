#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISFavorites
{
public:
    static ISFavorites& Instance();

    void Initialize(const QVariantMap &VariantMap);
    QVariantMap GetFavorites();
    void Add(const QString &TableName, unsigned int ObjectID); //Добавить объект в избранное
    void Delete(const QString &TableName, unsigned int ObjectID); //Удалить избранный объект
    void DeleteAll(); //Удалить все избранные объекты
    bool Exist(const QString &TableName, unsigned int ObjectID); //Проверить наличие объекта в избранном

private:
    ISFavorites();
    ~ISFavorites();
    ISFavorites(const ISFavorites&) = delete;
    ISFavorites(ISFavorites&&) = delete;
    ISFavorites& operator=(const ISFavorites&) = delete;
    ISFavorites& operator=(ISFavorites&&) = delete;

private:
    std::map<QString, ISVectorUInt> Favorites;
};
//-----------------------------------------------------------------------------
