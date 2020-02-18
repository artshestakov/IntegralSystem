#pragma once
#ifndef _PMETACLASSFOREIGN_H_INCLUDED
#define _PMETACLASSFOREIGN_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT PMetaClassForeign : public PMetaClass
{
	PMetaClassForeign() : PMetaClass("Foreign") { }

	QString Field; //Поле, на которое устанавливается внешний ключ
	QString ForeignClass; //На какую таблицу ссылкается внешний ключ
	QString ForeignField; //На какое поле ссылкается внешний ключ
	QString ForeignViewNameField; //Какое поле (поля) отображать в запросе на выборку
	QString OrderField; //Поле по которому будет происходить сортировка

	QString TableName; //Таблица, содержащая поле, на которое устанавливается внешний ключ
	QString SqlQuery; //Запрос на выбору по внешнему ключу
};
//-----------------------------------------------------------------------------
#endif
