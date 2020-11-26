#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISConfig
{
public:
	static ISConfig& Instance();

	QString GetErrorString() const;
	QString GetConfigPath() const;

	bool ReInitialize(const QString &template_name); //�����������������
	bool Initialize(const QString &template_name);
	
	QVariant GetValue(const QString &ParameterName); //�������� �������� ���������
	void SetValue(const QString &ParameterName, const QVariant &Value); //�������� �������� ���������
	void SaveForce(); //�������������� ����������

private:
	bool ReadXML(ISStringMap &StringMap); //������ XML-�������
	bool Update(); //���������� �����
	bool Create(); //��������� ����� �� �������

private:
	bool ContainsKey(const QString &Key); //��������� ������� ����� � �������

private:
	ISConfig();
	~ISConfig();
	ISConfig(ISConfig const &) {};
	ISConfig& operator=(ISConfig const&) { return *this; };

private:
	std::vector<ISConfigParameter> VectorTemplate;
	QString ErrorString;
	int ErrorLine;
	int ErrorColumn;
	QSettings *Settings;
	QString TemplateName;
	QDomNode DomNodeTemplate;
	QString PathConfigFile;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME) //�������� �������� ��������� �� ����������������� �����
#define CONFIG_BOOL(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toBool() //�������� �������� ���� bool
#define CONFIG_STRING(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toString() //�������� �������� ���� QString
#define CONFIG_INT(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toInt() //�������� �������� ���� int
//-----------------------------------------------------------------------------
#endif
