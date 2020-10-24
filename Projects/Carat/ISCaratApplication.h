#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
class ISCaratApplication : public QCoreApplication
{
	Q_OBJECT

public:
	ISCaratApplication(int argc, char **argv);
	virtual ~ISCaratApplication();

	QString GetErrorString() const; //Получить текстовое описание ошибки
	bool Init(); //Инициализация
	bool Run(const QStringList &Arguments); //Запуск с параметрами
	bool Run(); //Стандартный запуск

private:
	void Version();
	void Help();

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
#endif
