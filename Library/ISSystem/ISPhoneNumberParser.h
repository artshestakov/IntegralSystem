#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISPhoneNumberParser : public QObject
{
	Q_OBJECT

public:
	ISPhoneNumberParser(QObject *parent = 0);
	virtual ~ISPhoneNumberParser();

	static QString RemoveNotDigits(const QString &String); //�������� �� ���� �� ������
	static QString PasteEvent(const QString &PastedString); //������� ����������� ������
};
//-----------------------------------------------------------------------------
