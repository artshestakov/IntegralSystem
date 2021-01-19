#pragma once
#ifndef _ISCONSOLE_H_INCLUDED
#define _ISCONSOLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISConsole
{
public:
	static bool Question(const QString &QuestionText); //Задать вопрос пользователю
	static void Pause(); //Вызов паузы
	static QString GetString(const QString &OutputString = QString()); //Запрос на ввод текста
	static int GetInt(const QString &OutputString = QString()); //Запрос на ввод целого числа
	static bool InstallEncoding(unsigned int CodePage); //Установить кодовую страницу
};
//-----------------------------------------------------------------------------
#endif
