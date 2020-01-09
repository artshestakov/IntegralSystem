#pragma once
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
	void database(); //Обновление базы данных
	void functions(); //Обновление функций
	void tables(); //Обновление таблиц
	void systemindexes(); //Обновление системных индексов
	void indexes(); //Обновление индексов
	void compoundindexes(); //Обновление составных индексов
	void foreigns(); //Обновление внешних ключей
	void resources(); //Обновление ресурсов
	void classes(); //Обновление описания таблиц
	void classfields(); //Обновление описания полей таблиц
};
//-----------------------------------------------------------------------------
