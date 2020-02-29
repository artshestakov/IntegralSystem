#pragma once
#ifndef _ISCONSOLE_H_INCLUDED
#define _ISCONSOLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISConsole
{
public:
	static bool Question(const QString &QuestionText); //Задать вопрос пользователю
	static void Pause(); //Вызов паузы
	static QString GetString(const QString &OutputString = QString()); //Запрос на ввод текста
	static int GetInt(const QString &OutputString = QString()); //Запрос на ввод целого числа
};
//-----------------------------------------------------------------------------
#endif
