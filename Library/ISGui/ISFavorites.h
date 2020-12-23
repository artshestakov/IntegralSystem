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
	ISFavorites(ISFavorites const &) {};
	ISFavorites& operator=(ISFavorites const&) { return *this; };

private:
	std::map<QString, ISVectorUInt> Favorites;
};
//-----------------------------------------------------------------------------
