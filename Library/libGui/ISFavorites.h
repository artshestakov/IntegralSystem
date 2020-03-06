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
	void AddFavorite(const QString &TableName, const QString &TableLocalName, const QString &ObjectName, int ObjectID); //�������� ������ � ���������
	bool DeleteFavorite(const QString &TableName, int ObjectID); //������� ��������� ������
	void DeleteAllFavorites(); //������� ��� ��������� �������
	bool CheckExistFavoriteObject(const QString &TableName, int ObjectID); //��������� ������� ������� � ���������

private:
	ISFavorites();

	QMap<QString, ISVectorInt> Favorites;
};
//-----------------------------------------------------------------------------
