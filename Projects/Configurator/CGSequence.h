#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGSequence : public QObject
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
