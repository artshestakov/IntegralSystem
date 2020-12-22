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
	void AddObject(const QString &TableName, int ObjectID); //Добавить объект в историю
	void Clear(); //Удалить все объекты из истории
	std::vector<ISHistoryObject>& GetHistory(); //Получить всю историю
	bool Save(); //Сохранить в БД

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
