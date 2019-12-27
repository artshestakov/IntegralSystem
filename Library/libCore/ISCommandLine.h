#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCommandLine
{
public:
	ISCommandLine();
	virtual ~ISCommandLine();

	static bool Question(const QString &QuestionText); //������ ������ ������������
	static void Pause(); //����� �����
	static QString GetText(); //������ �� ���� ������
};
//-----------------------------------------------------------------------------
