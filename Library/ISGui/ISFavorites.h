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
	void AddFavorite(const QString &TableName, int ObjectID); //�������� ������ � ���������
	void DeleteFavorite(const QString &TableName, int ObjectID); //������� ��������� ������
	void DeleteAllFavorites(); //������� ��� ��������� �������
	bool CheckExistFavoriteObject(const QString &TableName, int ObjectID); //��������� ������� ������� � ���������
	ISVectorInt& GetObjects(const QString &TableName); //�������� ��������� ������� �� �������
	std::map<QString, ISVectorInt>& GetObjects(); //�������� ��� ��������� �������
	bool Save(); //��������� � ��

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
