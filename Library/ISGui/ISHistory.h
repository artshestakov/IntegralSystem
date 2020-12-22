#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISHistory
{
public:
	static ISHistory& Instance();

	QString GetErrorString() const;
	void Initialize(const QVariantList &VariantList);
	void AddObject(const QString &TableName, int ObjectID); //�������� ������ � �������
	void Clear(); //������� ��� ������� �� �������
	std::vector<ISHistoryObject>& GetHistory(); //�������� ��� �������
	bool Save(); //��������� � ��

private:
	ISHistory();
	~ISHistory();
	ISHistory(ISHistory const &) {};
	ISHistory& operator=(ISHistory const&) { return *this; };

private:
	QString ErrorString;
	std::vector<ISHistoryObject> Stories;
};
//-----------------------------------------------------------------------------
