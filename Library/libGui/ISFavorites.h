#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISFavorites
{
public:
	ISFavorites(const ISFavorites &) = delete;
	ISFavorites(ISFavorites &&) = delete;
	ISFavorites &operator=(const ISFavorites &) = delete;
	ISFavorites &operator=(ISFavorites &&) = delete;
	~ISFavorites();

	static ISFavorites& GetInstance();

	void Initialize();
	void AddFavorite(const QString &TableName, const QString &TableLocalName, const QString &ObjectName, int ObjectID); //Добавить объект в избранное
	bool DeleteFavorite(const QString &TableName, int ObjectID); //Удалить избранный объект
	void DeleteAllFavorites(); //Удалить все избранные объекты
	bool CheckExistFavoriteObject(const QString &TableName, int ObjectID); //Проверить наличие объекта в избранном

private:
	ISFavorites();

	QMap<QString, ISVectorInt> Favorites;
};
//-----------------------------------------------------------------------------
