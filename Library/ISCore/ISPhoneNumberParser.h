#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISPhoneNumberParser
{
public:
	static QString RemoveNotDigits(const QString &String); //�������� �� ���� �� ������
	static QString PasteEvent(const QString &PastedString); //������� ����������� ������
};
//-----------------------------------------------------------------------------
