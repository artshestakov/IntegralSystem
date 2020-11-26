#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISConfig
{
public:
	static ISConfig& Instance();

	QString GetErrorString() const;
	QString GetConfigPath() const;

	bool ReInitialize(const QString &template_name); //Переинициализация
	bool Initialize(const QString &template_name);
	
	QVariant GetValue(const QString &ParameterName); //Получить значение параметра
	void SetValue(const QString &ParameterName, const QVariant &Value); //Изменить значение параметра
	void SaveForce(); //Принудительное сохранение

private:
	bool ReadXML(ISStringMap &StringMap); //Чтение XML-шаблона
	bool Update(const ISStringMap &StringMap); //Обновление файла
	bool Create(const ISStringMap &StringMap); //Генерация файла из шаблона

private:
	ISConfig();
	~ISConfig();
	ISConfig(ISConfig const &) {};
	ISConfig& operator=(ISConfig const&) { return *this; };

private:
	QString ErrorString;
	int ErrorLine;
	int ErrorColumn;
	QSettings *Settings;
	QString TemplateName;
	QString PathConfigTemplate;
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
