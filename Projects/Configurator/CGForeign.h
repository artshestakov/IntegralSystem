#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class CGForeign
{
public:
	static bool CreateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //Создание внешнего ключа
	static bool UpdateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //Обновление внешнего ключа
	static bool CheckExistForeign(PMetaClassForeign *MetaForeign); //Проверка существования внешнего ключа

private:
	static QString OnGetForeignName(PMetaClassForeign *MetaForeign); //Получение имени внешнего ключа
};
//-----------------------------------------------------------------------------
