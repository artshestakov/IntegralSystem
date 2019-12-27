#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISProperty
{
public:
	ISProperty(const ISProperty &) = delete;
	ISProperty(ISProperty &&) = delete;
	ISProperty &operator=(const ISProperty &) = delete;
	ISProperty &operator=(ISProperty &&) = delete;
	~ISProperty();

	static ISProperty& GetInstance(); //�������� ��������
	
	void SetValue(const QString &PropertyName, const QVariant &Value); //�������� ��������
	QVariant GetValue(const QString &PropertyName) const; //�������� �������� �� �����
	bool GetValueBool(const QString &PropertyName) const;
	void AddProperty(const QString &PropertyName, const QVariant &Value); //�������� ��������

private:
	ISProperty();

	QVariantMap Properties;
};
//-----------------------------------------------------------------------------
