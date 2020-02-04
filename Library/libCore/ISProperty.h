#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISProperty
{
public:
	ISProperty(const ISProperty &) = delete;
	ISProperty(ISProperty &&) = delete;
	ISProperty &operator=(const ISProperty &) = delete;
	ISProperty &operator=(ISProperty &&) = delete;
	~ISProperty();

	static ISProperty& GetInstance(); //Получить свойства
	
	void SetValue(const QString &PropertyName, const QVariant &Value); //Изменить свойство
	QVariant GetValue(const QString &PropertyName) const; //Получить свойство по имени
	bool GetValueBool(const QString &PropertyName) const;
	void AddProperty(const QString &PropertyName, const QVariant &Value); //Добавить свойство

private:
	ISProperty();

	QVariantMap Properties;
};
//-----------------------------------------------------------------------------
