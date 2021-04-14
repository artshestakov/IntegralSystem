#pragma once
#ifndef _ISCONSOLE_H_INCLUDED
#define _ISCONSOLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISConsole
{
public:
	static bool Question(const std::string &QuestionText); //Задать вопрос пользователю
	static void Pause(); //Вызов паузы
	static std::string GetString(const std::string &OutputString = std::string()); //Запрос на ввод текста
	static int GetInt(const std::string &OutputString = std::string()); //Запрос на ввод целого числа
	static bool InstallEncoding(unsigned int CodePage, std::string &ErrorString); //Установить кодовую страницу
};
//-----------------------------------------------------------------------------
#endif
