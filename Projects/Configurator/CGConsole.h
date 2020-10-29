#pragma once
#ifndef _CGCONSOLE_H_INCLUDED
#define _CGCONSOLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGConsole
{
public:
	static bool Question(const QString &QuestionText); //Задать вопрос пользователю
	static void Pause(); //Вызов паузы
	static QString GetString(const QString &OutputString = QString()); //Запрос на ввод текста
	static int GetInt(const QString &OutputString = QString()); //Запрос на ввод целого числа
};
//-----------------------------------------------------------------------------
#endif
