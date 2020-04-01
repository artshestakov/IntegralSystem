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
	QVariant GetValue(const QString &PropertyName) const; //�������� �������� �������� �� �����

private:
	ISProperty();
	~ISProperty();
	ISProperty(ISProperty const &) {};
	ISProperty& operator=(ISProperty const&) { return *this; };

private:
	ISStringToVariantMap Map;
};
//-----------------------------------------------------------------------------
#endif
