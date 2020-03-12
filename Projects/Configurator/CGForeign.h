#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGForeign
{
public:
	static bool CreateForeign(PMetaForeign *MetaForeign, QString &ErrorString); //Создание внешнего ключа
	static bool UpdateForeign(PMetaForeign *MetaForeign, QString &ErrorString); //Обновление внешнего ключа
	static bool CheckExistForeign(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString); //Проверка существования внешнего ключа

private:
	static QString GetForeignName(PMetaForeign *MetaForeign); //Получение имени внешнего ключа
};
//-----------------------------------------------------------------------------
