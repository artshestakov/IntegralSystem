#pragma once
#ifndef _ISPROPERTY_H_INCLUDED
#define _ISPROPERTY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISProperty
{
public:
	static ISProperty& Instance(); //�������� ��������
	
	void SetValue(const QString &PropertyName, const QVariant &Value); //�������� ��������
	QVariant GetValue(const QString &PropertyName); //�������� �������� �������� �� �����

private:
	ISProperty();
	~ISProperty();
	ISProperty(ISProperty const &) {};
	ISProperty& operator=(ISProperty const&) { return *this; };

private:
	ISStringToVariantMap Map;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define PROPERTY_GET(PROPERTY_NAME) ISProperty::Instance().GetValue(PROPERTY_NAME)
#define PROPERTY_SET(PROPERTY_NAME, PROPERTY_VALUE) ISProperty::Instance().SetValue(PROPERTY_NAME, PROPERTY_VALUE)
//-----------------------------------------------------------------------------
#endif
