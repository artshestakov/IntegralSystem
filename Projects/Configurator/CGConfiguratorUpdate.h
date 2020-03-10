#pragma once
#ifndef _CGCONFIGURATORUPDATE_H_INCLUDED
#define _CGCONFIGURATORUPDATE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorUpdate : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorUpdate();
	virtual ~CGConfiguratorUpdate();

public slots:
	bool database(); //Обновление базы данных
	bool functions(); //Обновление функций
	bool tables(); //Обновление таблиц
	bool indexesall(); //Обновление всех индексов
	bool systemindexes(); //Обновление системных индексов
	bool indexes(); //Обновление индексов
	bool compoundindexes(); //Обновление составных индексов
	void foreigns(); //Обновление внешних ключей
	void resources(); //Обновление ресурсов
	void systemuser(); //Обновление системного пользователя
	void databasesettings();
};
//-----------------------------------------------------------------------------
#endif
