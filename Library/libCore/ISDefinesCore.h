#pragma once
#ifndef _ISDEFINESCORE_H_INCLUDED
#define _ISDEFINESCORE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISDefinesCore
{
public:
	static ISDefinesCore& Instance();
	void Init(bool IsGui); //Инициализация

private:
	ISDefinesCore();
	~ISDefinesCore();
	ISDefinesCore(const ISDefinesCore &);
	ISDefinesCore& operator=(const ISDefinesCore &);

public:
	bool IS_GUI; //Флаг наличия графического интерфейса
	QString APPLICATION_NAME;
	QString PATH_APPLICATION_DIR;
	QString PATH_APPLICATION_FILE;
	QString PATH_LOGS_DIR;
	QString PATH_TEMP_DIR;
	QString PATH_TRANSLATIONS_DIR;
	QString PATH_CONFIG_FILE;
	QString SYMBOL_CIRCLE; //Символ "Круг"
	QString SYMBOL_MULTIPLY; //Символ "Умножение"
	QString SYMBOL_DIVIDE; //Символ "Деление"
	QString SYMBOL_OBJECT_CHANGED;
	QString SYMBOL_SPACE_HIDE; //Невидмый символ (НЕ ЯВЛЯЕТСЯ СТАНДАРТНЫМ СИМВОЛОМ ПРОБЕЛА)
	QString SYMBOL_FIAS_SPLIT;
};
//-----------------------------------------------------------------------------
#define DEFINES_CORE ISDefinesCore::Instance()
//-----------------------------------------------------------------------------
#endif
