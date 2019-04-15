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

	static bool CreateSequence(const QString &ClassName); //�������� ������������������
	static bool CheckExistSequence(const QString &ClassName); //�������� ������������� ������������������
	static QString GetSequenceNameForClass(const QString &ClassName); //��������� ����� ������������������
};
//-----------------------------------------------------------------------------
