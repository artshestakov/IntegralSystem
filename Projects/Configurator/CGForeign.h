#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGForeign
{
public:
	static bool CreateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //Создание внешнего ключа
	static bool UpdateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //Обновление внешнего ключа
	static bool CheckExistForeign(PMetaClassForeign *MetaForeign, bool &Exist, QString &ErrorString); //Проверка существования внешнего ключа

private:
	static QString GetForeignName(PMetaClassForeign *MetaForeign); //Получение имени внешнего ключа
};
//-----------------------------------------------------------------------------
