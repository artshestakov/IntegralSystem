#pragma once
#ifndef _ISALGORITHMOLD_H_INCLUDED
#define _ISALGORITHMOLD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISSettingsDatabase.h"
#include "ISTypedefsOld.h"
//-----------------------------------------------------------------------------

//ќбработка дробного значени€
#define DOUBLE_PREPAREN(d, n) QString::number(d, 'f', n)
#define DOUBLE_PREPARE(d) DOUBLE_PREPAREN(d, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA))

//ќбработка денежного значени€ (кол-во знаков после зап€той должно быть 2)
#define DOUBLE_PREPAREM(d) QString::number(d, 'f', 2)

//ќбработать событи€ из очереди событий
#define PROCESS_EVENTS qApp->processEvents

//-----------------------------------------------------------------------------
#endif
