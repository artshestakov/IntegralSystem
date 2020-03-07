#pragma once
#ifndef _ISPROPERTY_H_INCLUDED
#define _ISPROPERTY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISProperty
{
public:
	static ISProperty& Instance(); //Получить свойства
	
	void SetValue(const QString &PropertyName, const QVariant &Value); //Изменить свойство
	QVariant GetValue(const QString &PropertyName) const; //Получить значение свойства по имени

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
