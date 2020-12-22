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
	void AddFavorite(const QString &TableName, unsigned int ObjectID); //�������� ������ � ���������
	void DeleteFavorite(const QString &TableName, unsigned int ObjectID); //������� ��������� ������
	void DeleteAllFavorites(); //������� ��� ��������� �������
	bool CheckExistFavoriteObject(const QString &TableName, unsigned int ObjectID); //��������� ������� ������� � ���������
	ISVectorInt& GetObjects(const QString &TableName); //�������� ��������� ������� �� �������
	std::map<QString, ISVectorInt>& GetObjects(); //�������� ��� ��������� �������

private:
	ISFavorites();
	~ISFavorites();
	ISFavorites(ISFavorites const &) {};
	ISFavorites& operator=(ISFavorites const&) { return *this; };

private:
	std::map<QString, ISVectorUInt> Favorites;
};
//-----------------------------------------------------------------------------
