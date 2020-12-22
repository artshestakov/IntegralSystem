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
	bool Initialize();
	void AddFavorite(const QString &TableName, unsigned int ObjectID); //Добавить объект в избранное
	void DeleteFavorite(const QString &TableName, unsigned int ObjectID); //Удалить избранный объект
	void DeleteAllFavorites(); //Удалить все избранные объекты
	bool CheckExistFavoriteObject(const QString &TableName, unsigned int ObjectID); //Проверить наличие объекта в избранном
	ISVectorInt& GetObjects(const QString &TableName); //Получить избранные объекты по таблице
	std::map<QString, ISVectorInt>& GetObjects(); //Получить все избранные объекты

private:
	ISFavorites();
	~ISFavorites();
	ISFavorites(ISFavorites const &) {};
	ISFavorites& operator=(ISFavorites const&) { return *this; };

private:
	std::map<QString, ISVectorUInt> Favorites;
};
//-----------------------------------------------------------------------------
