#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISPassword
{
public:
	ISPassword();
	virtual ~ISPassword();

	static QString GeneratePassword(); //������������� ������
	static int GetWidthPassword(const QString &Password); //�������� "���" ������

protected:
	static int GetCountFigures(const QString &Password); //�������� ���������� ���� � ������
	static int GetCountAltSymbols(const QString &Password); //�������� ���������� �������������� ��������
	static int GetCountUpperLiters(const QString &Password); //�������� ���������� ���� � ������� ��������
};
//-----------------------------------------------------------------------------
