#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISPassword : public QObject
{
	Q_OBJECT

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
