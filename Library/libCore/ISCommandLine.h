#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCommandLine
{
public:
	static bool Question(const QString &QuestionText); //Задать вопрос пользователю
	static void Pause(); //Вызов паузы
	static QString GetText(); //Запрос на ввод текста
};
//-----------------------------------------------------------------------------
