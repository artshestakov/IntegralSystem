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
	void Add(const QString &TableName, unsigned int ObjectID); //�������� ������ � ���������
	void Delete(const QString &TableName, unsigned int ObjectID); //������� ��������� ������
	void DeleteAll(); //������� ��� ��������� �������
	bool Exist(const QString &TableName, unsigned int ObjectID); //��������� ������� ������� � ���������

private:
	ISFavorites();
	~ISFavorites();
	ISFavorites(ISFavorites const &) {};
	ISFavorites& operator=(ISFavorites const&) { return *this; };

private:
	std::map<QString, ISVectorUInt> Favorites;
};
//-----------------------------------------------------------------------------
