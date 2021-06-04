#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructsOld.h"
//-----------------------------------------------------------------------------
class ISConfig
{
public:
	static ISConfig& Instance();

	QString GetErrorString() const;
	bool IsValid(); //Проверить корректность заполнения конфигурационного файла

	bool ReInitialize(const QString &template_name); //Переинициализация
	bool Initialize(const QString &template_name);
	
	QVariant GetValue(const QString &ParameterName); //Получить значение параметра
	void SetValue(const QString &ParameterName, const QVariant &Value); //Изменить значение параметра
	void SaveForce(); //Принудительное сохранение

private:
	bool Update(); //Обновление файла
	bool Create(); //Генерация файла из шаблона

private:
	bool ContainsKey(const QString &Key); //Проверить наличие ключа в шаблоне
	QVariant GetDefaultValue(const QString &Key) const; //Получить значение по умолчанию для параметра

private:
	ISConfig();
	~ISConfig();
	ISConfig(const ISConfig&) = delete;
	ISConfig(ISConfig&&) = delete;
	ISConfig& operator=(const ISConfig&) = delete;
	ISConfig& operator=(ISConfig&&) = delete;

private:
	std::vector<ISConfigParameter> VectorTemplate;
	QString ErrorString;
	int ErrorLine;
	int ErrorColumn;
	QSettings *Settings;
	ISStringToVariantMap SettingsMap;
	QString TemplateName;
	QDomNode DomNodeTemplate;
	QString PathConfigFile;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME) //Получить значение параметра из конфигурационного файла
#define CONFIG_BOOL(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toBool() //Получить значение типа bool
#define CONFIG_STRING(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toString() //Получить значение типа QString
#define CONFIG_INT(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toInt() //Получить значение типа int
//-----------------------------------------------------------------------------
#endif
