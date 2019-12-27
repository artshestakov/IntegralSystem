#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISCommandLine
{
public:
	ISCommandLine();
	virtual ~ISCommandLine();

	static bool Question(const QString &QuestionText); //Задать вопрос пользователю
	static void Pause(); //Вызов паузы
	static QString GetText(); //Запрос на ввод текста
};
//-----------------------------------------------------------------------------
