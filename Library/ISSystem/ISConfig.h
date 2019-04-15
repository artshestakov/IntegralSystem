#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISConfig : public QObject
{
	Q_OBJECT

public:
	ISConfig(const ISConfig &) = delete;
	ISConfig(ISConfig &&) = delete;
	ISConfig &operator=(const ISConfig &) = delete;
	ISConfig &operator=(ISConfig &&) = delete;
	~ISConfig();

	static ISConfig& GetInstance();

	void Initialize(const QString &ConfigFilePath);
	
	QVariant GetValue(const QString &ParameterName); //Получить значение параметра
	bool GetValueBool(const QString &ParameterName);
	QString GetValueString(const QString &ParameterName);
	int GetValueInt(const QString &ParameterName);

	void SetValue(const QString &ParameterName, const QVariant &Value); //Изменить значение параметра
	void ClearValue(const QString &ParameterName); //Очистить значение параметра

private:
	ISConfig();

	QSettings *Settings;
};
//-----------------------------------------------------------------------------
#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::GetInstance().GetValue(PARAMETER_NAME)
#define CONFIG_BOOL(PARAMETER_NAME) ISConfig::GetInstance().GetValueBool(PARAMETER_NAME)
#define CONFIG_STRING(PARAMETER_NAME) ISConfig::GetInstance().GetValueString(PARAMETER_NAME)
#define CONFIG_INT(PARAMETER_NAME) ISConfig::GetInstance().GetValueInt(PARAMETER_NAME)
//-----------------------------------------------------------------------------
