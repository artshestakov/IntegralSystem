#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISCommandLine
{
public:
	ISCommandLine();
	virtual ~ISCommandLine();

	static bool Question(const QString &QuestionText); //������ ������ ������������
	static void Pause(); //����� �����
	static QString GetText(); //������ �� ���� ������
};
//-----------------------------------------------------------------------------
