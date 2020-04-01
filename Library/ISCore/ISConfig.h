#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISConfig
{
public:
	static ISConfig& Instance();

	QString GetErrorString() const;
	bool Initialize();
	
	QVariant GetValue(const QString &ParameterName); //Получить значение параметра
	void SetValue(const QString &ParameterName, const QVariant &Value); //Изменить значение параметра
	void SaveForce(); //Принудительное сохранение

private:
	bool Update(); //Обновление файла
	bool Create(); //Генерация файла из шаблона

private:
	ISConfig();
	~ISConfig();
	ISConfig(ISConfig const &) {};
	ISConfig& operator=(ISConfig const&) { return *this; };

private:
	QString ErrorString;
	QSettings *Settings;
	QString PathConfigTemplate;
};
//-----------------------------------------------------------------------------
//Получить значение параметра из конфигурационного файла
#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME)

//Получить значение типа bool
#define CONFIG_BOOL(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toBool()

//Получить значение типа QString
#define CONFIG_STRING(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toString()

//Получить значение типа int
#define CONFIG_INT(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toInt()
//-----------------------------------------------------------------------------
#endif
