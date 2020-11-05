#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISFavorites
{
public:
	static ISFavorites& Instance();

	QString GetErrorString() const;
	void Initialize(const QVariantMap &VariantMap);
	bool Initialize();
	void AddFavorite(const QString &TableName, int ObjectID); //Добавить объект в избранное
	void DeleteFavorite(const QString &TableName, int ObjectID); //Удалить избранный объект
	void DeleteAllFavorites(); //Удалить все избранные объекты
	bool CheckExistFavoriteObject(const QString &TableName, int ObjectID); //Проверить наличие объекта в избранном
	ISVectorInt& GetObjects(const QString &TableName); //Получить избранные объекты по таблице
	std::map<QString, ISVectorInt>& GetObjects(); //Получить все избранные объекты
	bool Save(); //Сохранить в БД

private:
	ISFavorites();
	~ISFavorites();
	ISFavorites(ISFavorites const &) {};
	ISFavorites& operator=(ISFavorites const&) { return *this; };

private:
	QString ErrorString;
	std::map<QString, ISVectorInt> Favorites;
};
//-----------------------------------------------------------------------------
