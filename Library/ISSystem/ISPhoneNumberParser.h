#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISPhoneNumberParser
{
public:
	static QString RemoveNotDigits(const QString &String); //�������� �� ���� �� ������
	static QString PasteEvent(const QString &PastedString); //������� ����������� ������
};
//-----------------------------------------------------------------------------
