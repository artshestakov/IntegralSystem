#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISPhoneNumberParser
{
public:
	static QString RemoveNotDigits(const QString &String); //�������� �� ���� �� ������
	static QString PasteEvent(const QString &PastedString); //������� ����������� ������
};
//-----------------------------------------------------------------------------
