#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGSequence : public QObject
{
	Q_OBJECT

public:
	CGSequence();
	virtual ~CGSequence();

	static bool CreateSequence(const QString &ClassName); //Создание последовательности
	static bool CheckExistSequence(const QString &ClassName); //Проверка существования последовательности
	static QString GetSequenceNameForClass(const QString &ClassName); //Получение имени последовательности
};
//-----------------------------------------------------------------------------
