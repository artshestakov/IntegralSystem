#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGIndex
{
public:
	static bool CreateIndex(PMetaIndex *Index, QString &ErrorString); //Создание индекса
	static bool UpdateIndex(PMetaIndex *Index, QString &ErrorString); //Обновление индекса
	static bool CheckExistIndex(PMetaIndex *Index, bool &Exist, QString &ErrorString); //Проверка существования индекса
	static bool CheckIndexForeign(PMetaIndex *Index); //Проверить ссылается ли на этого индекс внешний ключ
	static bool ReindexIndex(PMetaIndex *Index, QString &ErrorString); //Перестроить индекс

private:
	static QString GetIndexName(PMetaIndex *Index); //Получение имени индекса
};
//-----------------------------------------------------------------------------
