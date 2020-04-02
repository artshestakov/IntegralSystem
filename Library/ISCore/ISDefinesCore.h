#pragma once
#ifndef _ISDEFINESCORE_H_INCLUDED
#define _ISDEFINESCORE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
namespace ISDefines
{
	class ISCORE_EXPORT Core
	{
	public:
		static void Init(bool IsGui); //Инициализация

		static bool IS_GUI; //Флаг наличия графического интерфейса
		static QString APPLICATION_NAME;
		static QString PATH_APPLICATION_DIR;
		static QString PATH_APPLICATION_FILE;
		static QString PATH_LOGS_DIR;
		static QString PATH_TEMP_DIR;
		static QString PATH_CRASH_DIR;
		static QString PATH_TRANSLATIONS_DIR;
		static QString PATH_CONFIG_FILE;
        static QString PATH_PID_FILE;
		static QString SYMBOL_CIRCLE; //Символ "Круг"
		static QString SYMBOL_MULTIPLY; //Символ "Умножение"
		static QString SYMBOL_DIVIDE; //Символ "Деление"
		static QString SYMBOL_OBJECT_CHANGED;
		static QString SYMBOL_SPACE_HIDE; //Невидмый символ (НЕ ЯВЛЯЕТСЯ СТАНДАРТНЫМ СИМВОЛОМ ПРОБЕЛА)
		static QString SYMBOL_FIAS_SPLIT;
	};
}
//-----------------------------------------------------------------------------
#endif
